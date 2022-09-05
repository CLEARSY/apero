/** ppTransNonIncr.cpp

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
#include "ppTransNonIncr.h"
#include "ppTrans.h"
#include "decomposition.h"
#include "utils.h"

#include <fstream>
#include <iostream>

namespace ppTransNonIncr {

    static void merge(std::set<std::string> &set, const std::set<std::string> &to_insert){
        for(auto &e : to_insert)
            set.insert(e);
    }

    static void merge(std::set<VarName> &set, const std::set<VarName> &to_insert){
        for(auto &e : to_insert)
            set.insert(e);
    }
    static bool is_intersecting(
            const std::set<VarName> &set1,
            const std::set<VarName> &set2)
    {
        if(set1.empty())
            return false;

        for(auto &v : set2){
            if(set1.find(v) != set1.end())
                return true;
        }
        return false;
    }

    static bool keepHyp(
            const std::set<VarName> &rpVars,
            const Pred &p)
    {
        if(rpVars.empty())
            return false;
        std::set<VarName> fv_p;
        p.getFreeVars({},fv_p);
        return is_intersecting(rpVars,fv_p);
    }

    static bool keepHyp(
            const std::set<VarName> &rpVars,
            const pog::Set &set)
    {
        if(rpVars.empty())
            return false;
        std::set<VarName> fv_set;
        fv_set.insert(set.setName.name);
        for(auto &e : set.elts)
            fv_set.insert(e.name);
        return is_intersecting(rpVars,fv_set);
    }

    static int findDef(
            const std::vector<pog::Define> &vec,
            const std::string &d)
    {
        int pos = -1;
        for(int i=0;i<vec.size();i++){
            if(vec[i].name == d){
                pos = i;
                break;
            }
        }
        assert(pos>=0); // exception
        return pos;
    }

    using translation_t = std::pair<std::string,std::set<std::string>>;

    static translation_t ppTrans(
            ppTrans::Context &env,
            const std::vector<pog::Define> &vec,
            const std::string &d)
    {
        int pos = findDef(vec,d);
        int nbChildren = vec[pos].gsets.size() + vec[pos].ghyps.size();
        if(nbChildren == 0){
            return {"; Definition " + vec[pos].name + " = true",{}};
        }
        std::ostringstream str;
        str << "; Definition " << vec[pos].name << "\n(assert";
        if(nbChildren > 1)
            str << " (and";
        std::set<std::string> used_ids;
        for(const auto& s : vec[pos].gsets){
            str << " ";
            ppTrans::ppTrans(str,env,s,used_ids);
        }
        for(const auto& h : vec[pos].ghyps){
            str << " ";
            ppTrans::ppTrans(str,env,h,used_ids);
        }
        if(nbChildren > 1)
            str << "))";
        else
            str << ")";
        return {str.str(),used_ids};
    }

    static void updateRpVars(
            std::set<VarName> &rpVars,
            const Pred &h)
    {
        if(rpVars.empty())
            return;
        std::set<VarName> fv_h;
        h.getFreeVars({},fv_h);
        if(is_intersecting(rpVars,fv_h))
            merge(rpVars,fv_h);
    }

    static void updateRpVars(
            std::set<VarName> &rpVars,
            const pog::Set &set)
    {
        if(rpVars.empty())
            return;
        std::set<VarName> fv_set;
        fv_set.insert(set.setName.name);
        for(auto &e : set.elts)
            fv_set.insert(e.name);
        if(is_intersecting(rpVars,fv_set))
            merge(rpVars,fv_set);
    }

    void saveSmtLibFileNonIncr(
            const pog::Pog &pog,
            ppTrans::Context &env,
            // Cache
            std::map<std::pair<std::string,int>,translation_t> &definitionHyps_tr,
            std::map<std::pair<std::string,int>,translation_t> &definitionSets_tr,
            std::map<int,translation_t> &globalHyps_tr,
            std::map<int,translation_t> &localHyps_tr,
            // PO
            int group_nb, int goal_nb,
            // Other
            int rp,
            bool dd,
            bool model,
            const std::string &filename,
            const std::string &minint,
            const std::string &maxint)
    {
        const pog::POGroup &group = pog.pos[group_nb];
        const pog::PO &sg = pog.pos[group_nb].simpleGoals[goal_nb];

        // Computing rpVars
        std::set<VarName> rpVars;
        if(rp >= 1){
            sg.goal.getFreeVars({},rpVars);
            if(!dd){
                for(auto &ref : sg.localHypsRef){
                    group.localHyps[ref-1].getFreeVars({},rpVars);
                }
            }
            for(int lrp=rp-1;lrp>0;lrp--){
                // defs
                for(auto &s : group.definitions){
                    const pog::Define &def = pog.defines[findDef(pog.defines,s)];
                    for(auto &h : def.ghyps)
                        updateRpVars(rpVars,h);
                    for(auto &h : def.gsets)
                        updateRpVars(rpVars,h);
                }
                // global hyps
                for(auto &h : group.hyps)
                    updateRpVars(rpVars,h);
                // local hyps
                if(dd){
                    for(auto &ref : sg.localHypsRef)
                        updateRpVars(rpVars,group.localHyps[ref-1]);
                }
            }
        }
        // ---

        std::set<std::string> used_ids;

        // definitions
        std::vector<std::string> defines;
        for(auto &def_name : group.definitions){
            const pog::Define &def = pog.defines[findDef(pog.defines,def_name)];
            for(int i=0;i<def.ghyps.size();i++){
                const Pred &hyp = def.ghyps[i];
                if(rp < 0 || keepHyp(rpVars,hyp)){
                    auto it = definitionHyps_tr.find({def_name,i});
                    if(it != definitionHyps_tr.end()){
                        merge(used_ids,it->second.second);
                        defines.push_back(it->second.first);
                    } else {
                        std::set<std::string> used_ids2;
                        std::ostringstream str;
                        ppTrans::ppTrans(str,env,hyp,used_ids2);
                        definitionHyps_tr[{def_name,i}] = {str.str(),used_ids2};
                        merge(used_ids,used_ids2);
                        defines.push_back(str.str());
                    }
                }
            }
            for(int i=0;i<def.gsets.size();i++){
                const pog::Set &set = def.gsets[i];
                if(rp < 0 || keepHyp(rpVars,set)){
                    auto it = definitionSets_tr.find({def_name,i});
                    if(it != definitionSets_tr.end()){
                        merge(used_ids,it->second.second);
                        defines.push_back(it->second.first);
                    } else {
                        std::set<std::string> used_ids2;
                        std::ostringstream str;
                        ppTrans::ppTrans(str,env,set,used_ids2);
                        definitionSets_tr[{def_name,i}] = {str.str(),used_ids2};
                        merge(used_ids,used_ids2);
                        defines.push_back(str.str());
                    }
                }
            }
        }

        // Global hyps
        std::vector<std::string> globalHyps;
        for(int ref=0;ref<group.hyps.size();ref++){
            const Pred &hyp = group.hyps[ref];
            if(rp < 0 || keepHyp(rpVars,hyp)){
                auto it = globalHyps_tr.find(ref);
                if(it != globalHyps_tr.end()){
                    merge(used_ids,it->second.second);
                    globalHyps.push_back(it->second.first);
                } else {
                    std::set<std::string> used_ids2;
                    std::ostringstream str;
                    ppTrans::ppTrans(str,env,hyp,used_ids2);
                    globalHyps_tr[ref] = {str.str(),used_ids2};
                    merge(used_ids,used_ids2);
                    globalHyps.push_back(str.str());
                }
            }
        }

        // Local Hyps
        std::vector<std::string> localHyps;
        for(auto &ref: sg.localHypsRef){
            if(rp < 0 || !dd || keepHyp(rpVars,group.localHyps[ref-1])){
                auto it = localHyps_tr.find(ref);
                if(it != localHyps_tr.end()){
                    merge(used_ids,it->second.second);
                    localHyps.push_back(it->second.first);
                } else {
                    std::set<std::string> used_ids2;
                    std::ostringstream str;
                    ppTrans::ppTrans(str,env,group.localHyps[ref-1],used_ids2);
                    localHyps_tr[ref] = {str.str(),used_ids2};
                    merge(used_ids,used_ids2);
                    localHyps.push_back(str.str());
                }
            }
        }

        // Goal
        std::ostringstream goal;
        ppTrans::ppTrans(goal,env,sg.goal,used_ids);

        // Printing
        std::ofstream out;
        out.open (filename);
        out << "(set-option :print-success false)\n";
        if(model)
            out << "(set-option :produce-models true)\n";
        out << "(set-logic AUFNIRA)\n";
        out << "; PO " << group_nb << " " << goal_nb << " \n";
        out << "; Group " << group.tag << "\n";
        out << "; Tag " << sg.tag << "\n";
        out << "; Prelude\n";
        ppTrans::printPrelude(out,minint,maxint);
        out << "; Global declarations\n";
        for(auto &s : used_ids){
            if(s != "mem0" && s != "mem1")
                out << env.getSmtLibDeclarations().find(s)->second << std::endl;
        }
        out << "; Defines\n";
        for(auto &d : defines)
            out << "(assert " << d << ")\n";
        out << "; Global hypotheses\n";
        for(auto &hyp: globalHyps)
            out << "(assert " << hyp << ")\n";
        out << "; Local hypotheses\n";
        for(auto &hyp: localHyps)
            out << "(assert " << hyp << ")\n";
        out << "; Goal\n";
        out << "(assert (not " << goal.str() << "))\n";
        out << "(check-sat)\n";
        if (model)
            out << "(get-model)\n";
        out << "(exit)";
        out.close();
    }

    void saveSmtLibFileNonIncrOne(
            pog::Pog &pog,
            const string &filename,
            // PO
            int groupIdx,
            int goalIdx,
            // Other
            int rp,
            bool dd,
            bool model,
            const string &minint,
            const string &maxint)
    {
        std::map<std::pair<std::string,int>,translation_t> definitionHyps_tr;
        std::map<std::pair<std::string,int>,translation_t> definitionSets_tr;
        std::map<int,translation_t> globalHyps_tr;
        std::map<int,translation_t> localHyps_tr;
        ppTrans::Context env;
        decomp::decompose(pog);
        std::string absolutefilename { utils::absoluteFilePath(filename) };
        saveSmtLibFileNonIncr( pog, env, definitionHyps_tr, definitionSets_tr,
                globalHyps_tr, localHyps_tr, groupIdx, goalIdx, rp, dd, model,
                absolutefilename, minint, maxint);
    }

    void saveSmtLibFileNonIncr(
            pog::Pog &pog,
            const std::string &filename,
            int rp,
            bool dd,
            bool model,
            const std::string &minint,
            const std::string &maxint)
    {
        using std::to_string;
        std::string prefix { utils::absoluteBasename(filename) };

        std::map<std::pair<std::string,int>,translation_t> definitionHyps_tr;
        std::map<std::pair<std::string,int>,translation_t> definitionSets_tr;
        ppTrans::Context env;
        decomp::decompose(pog);
        for(int group_nb=0;group_nb<pog.pos.size();group_nb++){
            std::map<int,translation_t> globalHyps_tr;
            std::map<int,translation_t> localHyps_tr;
            for(int po_nb=0;po_nb<pog.pos[group_nb].simpleGoals.size();po_nb++){
                std::string path { prefix + "-" + to_string(group_nb) + "-" + to_string(po_nb) + ".smt2" };
                saveSmtLibFileNonIncr( pog, env, definitionHyps_tr, definitionSets_tr,
                                       globalHyps_tr, localHyps_tr, group_nb, po_nb, rp, dd, model,
                                       path, minint, maxint);
            }
        }
    }
}
