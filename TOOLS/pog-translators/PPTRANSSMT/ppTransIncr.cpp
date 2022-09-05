/** ppTransIncr.cpp

   \copyright Copyright © CLEARSY 2022
   \license This file is part of ppTransSmt.

   ppTransSmt is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

    ppTransSmt is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ppTransSmt. If not, see <https://www.gnu.org/licenses/>.
*/
#include <fstream>
#include <iterator>

#include "ppTransIncr.h"
#include "decomposition.h"
#include "ppTrans.h"

namespace ppTransIncr {
    class ppTransPo {
        public:
            std::string group;
            std::vector<std::string> definitions;
            std::vector<std::string> hypotheses;
            std::vector<std::pair<int,std::string>> localHypotheses;
            std::vector<std::pair<int,std::string>> simpleGoals;
    };

    void ppTrans(std::ostringstream &str, ppTrans::Context &env, const Pred &p){
        std::set<std::string> dummy;
        return ppTrans::ppTrans(str,env,p,dummy);
    }

    void ppTrans(std::ostringstream &str, ppTrans::Context &env, const pog::Set &set){
        std::set<std::string> dummy;
        return ppTrans::ppTrans(str,env,set,dummy);
    }

    void ppTrans(std::ostringstream &str, ppTrans::Context &env, const pog::PO &sg){
        switch(sg.localHypsRef.size()){
            case 0:
                return ppTrans(str,env,sg.goal);
            case 1:
                {
                    str << "(=> lh_" << std::to_string(sg.localHypsRef[0]) << " ";
                    ppTrans(str,env,sg.goal);
                    str << ")";
                    return;
                }
            default:
                {
                    str << "(=> (and";
                    for(auto &i : sg.localHypsRef)
                        str << " lh_" << std::to_string(i);
                    str << ") ";
                    ppTrans(str,env,sg.goal);
                    str << ")";
                    return;
                }
        }
        assert(false); // unreachable
    }

    static void preamble(pog::Pog& pog,
                         ppTrans::Context& env,
                         std::vector<std::string>& defines,
                         std::map<std::string,int>& defArity)
    {
        decomp::decompose(pog);
        for (const auto &def : pog.defines){
            int nbChildren = 0;
            for(const auto &s : def.gsets){
                std::ostringstream str;
                str << "(define-fun |def_" << def.name << "_" << std::to_string(nbChildren) << "| () Bool ";
                ppTrans(str,env,s);
                str << ")";
                defines.push_back(str.str());
                nbChildren++;
            }
            for(const auto &e : def.ghyps){
                std::ostringstream str;
                str << "(define-fun |def_" << def.name << "_" << std::to_string(nbChildren) << "| () Bool ";
                ppTrans(str,env,e);
                str << ")";
                defines.push_back(str.str());
                nbChildren++;
            }
            defArity[def.name] = nbChildren;
        }

    }

    static void saveSmt(const std::string &filename,
                        bool model,
                        const std::string &minint,
                        const std::string &maxint,
                        const ppTrans::Context& env,
                        const std::vector<std::string>& defines,
                        const std::map<std::string,int>& defArity,
                        const std::vector<ppTransPo>& pos)
    {
        std::ofstream out;
        out.open (filename);

        out << "(set-logic AUFNIRA)\n";
        out << "(set-option :print-success false)\n";
        //out << "(set-option :produce-unsat-cores true)\n";
        if(model)
            out << "(set-option :produce-models true)\n";
        out << "; Prelude\n";
        ppTrans::printPrelude(out,minint,maxint);
        out << "; Global declarations\n";
        for(auto &p : env.getSmtLibDeclarations())
            out << p.second << "\n";
        out << "; Defines\n";
        for(const std::string &d : defines)
            out << d << "\n";
        for(const auto &po: pos){
            out << "; PO group " << po.group << " \n";
            if (pos.size() > 1) out << "(push 1)\n";
            for(const auto &def: po.definitions){
                for(int i=0;i<defArity.at(def);i++)
                    out << "(assert (! |def_" << def << "_" << std::to_string(i)
                        << "| :named |" << def << std::to_string(i) << "|))\n";
            }
            int hcpt = 0;
            for(const auto &hyp: po.hypotheses){
                out << "(assert (! " << hyp << " :named gh"+std::to_string(hcpt)+"))\n";
                hcpt++;
            }
            for(const auto &lh: po.localHypotheses){
                out << "(define-fun lh_" << lh.first << " () Bool " << lh.second << ")\n";
            }
            for(const auto &sg: po.simpleGoals){
                out << "; PO " << sg.first << " in group " << po.group << "\n";
                if (po.simpleGoals.size() > 1) out << "(push 1)\n";
                out << "(assert (! (not " << sg.second << ") :named goal))\n";
                out << "(check-sat)\n";
                if (model)
                    out << "(get-model)\n";
                //            out << "(get-unsat-core)\n";
                if (po.simpleGoals.size() > 1) out << "(pop 1)\n";
            }
            if (pos.size() > 1) out << "(pop 1)\n";
        }
        out << "(exit)";
        out.close();

    }

    void saveSmtLibFileIncrSome
    (pog::Pog &pog,
     const std::string &filename,
     map<int, vector<int>>& goals,
     bool model,
     const std::string &minint,
     const std::string &maxint)
    {
        ppTrans::Context env;

        // Define Tags
        std::vector<std::string> defines;
        std::map<std::string,int> defArity;

        preamble(pog, env, defines, defArity);

        int group = 0;

        // only pos referenced in goals are ppTrans-lated.
        std::vector<ppTransPo> pos;
        for (const auto &po : pog.pos){
            auto it { goals.find(group) };
            // the current group of POs is not referenced in goals
            ++group;
            if(it == goals.end())
                continue;
            vector<int>& simple_goal_ids { it->second };
            // the current group of POs is empty
            if(simple_goal_ids.size() == 0)
                continue;

            ppTransPo ppo;
            ppo.group = po.tag;
            ppo.definitions = po.definitions;

            for(const auto &h : po.hyps){
                std::ostringstream str;
                ppTrans(str,env,h);
                ppo.hypotheses.push_back(str.str());
            }

            int i = 1;
            for(const auto &h : po.localHyps){
                std::ostringstream str;
                ppTrans(str,env,h);
                ppo.localHypotheses.push_back({i,str.str()});
                ++i;
            }

            // the POs referenced in goals are sorted so that
            // looking for goals to be translated has a linear cost.
            std::sort(simple_goal_ids.begin(), simple_goal_ids.end());

            int sgNb = 0;
            auto simple_goal_it { std::begin(simple_goal_ids) };
            for(const auto &sg : po.simpleGoals){
                // all goals in current group have been processed
                if (simple_goal_it == std::end(simple_goal_ids))
                    break;
                // current goal is in current group of POs : process it
                if(sgNb == *simple_goal_it) {
                    std::ostringstream str;
                    ppTrans(str,env,sg);
                    ppo.simpleGoals.push_back( {sgNb,str.str()} );
                    ++simple_goal_it;
                }
                sgNb++;
            }
            pos.push_back(ppo);
        }

        saveSmt(filename, model, minint, maxint, env, defines, defArity, pos);
    }

    void saveSmtLibFileIncr(pog::Pog &pog,
            const std::string &filename,
            bool model,
            const std::string &minint,
            const std::string &maxint)
    {
        ppTrans::Context env;

        // Define Tags
        std::vector<std::string> defines;
        std::map<std::string,int> defArity;

        preamble(pog, env, defines, defArity);

        int group = -1;

        std::vector<ppTransPo> pos;
        for (const auto &po : pog.pos){
            ++group;

            ppTransPo ppo;
            ppo.group = po.tag;
            ppo.definitions = po.definitions;

            for(const auto &h : po.hyps){
                std::ostringstream str;
                ppTrans(str,env,h);
                ppo.hypotheses.push_back(str.str());
            }

            int sgNb = 1;
            int i = 1;
            for(const auto &h : po.localHyps){
                std::ostringstream str;
                ppTrans(str,env,h);
                ppo.localHypotheses.push_back({i,str.str()});
                ++i;
            }
            for(const auto &sg : po.simpleGoals){
                std::ostringstream str;
                ppTrans(str,env,sg);
                ppo.simpleGoals.push_back( {sgNb,str.str()} );
                sgNb++;
            }
            pos.push_back(ppo);
        }

        saveSmt(filename, model, minint, maxint, env, defines, defArity, pos);
    }

}
