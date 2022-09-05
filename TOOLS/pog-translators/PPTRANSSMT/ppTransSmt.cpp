/** ppTransSmt.cpp

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
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<map>
#include<utility>
#include<vector>

using std::map;
using std::pair;
using std::vector;

#include<QDomDocument>
#include<QFile>
#include <QCoreApplication>
#include<QDebug>

#include "pog.h"
#include "ppTransIncr.h"
#include "ppTransNonIncr.h"

static void display_help()
{
    using std::cout;
    using std::endl;
    cout << "Translates Atelier B proof obligation file to SMT-LIB format." << endl
         << "\tppTransSmt [-m] -i infile.pog -o outfile" << endl
         << "\tppTransSmt [-n] [-a N1 M1 -a N2 M2 ... -a Nk Mk] [-m] [-rp R|-ddrp R] -i infile.pog -o outfile" << endl
         << "\t\t-n" << endl
         << "\t\t\tSets non-incremental mode : all POs are translated, one SMT file per PO is generated." << endl
         << "\t\t\tOtherwise ppTransSmt is in default mode: a single SMT file is generated." << endl
         << "\t\t-i infile.pog" << endl
         << "\t\t\tSpecifies the path for the input POG file." << endl
         << "\t\t-o output" << endl
         << "\t\t\tdefault mode: path for the output file." << endl
         << "\t\t\tnon-incremental mode: path pattern for the output files." << endl
         << "\t\t-m" << endl
         << "\t\t\tShows model." << endl
         << "\t\t-a N M" << endl
         << "\t\t\tDefault mode only." << endl
         << "\t\t\tSelects the N-th Simple_Goal child element from the M-th Proof_Obligation" << endl
         << "\t\t\telement for translation." << endl
         << "\t\t\tIf several goals are selected, the SMT produced will contain push/pop statements." << endl
         << "\t\t-rp R" << endl
         << "\t\t\tSelects hypothesis that belong to rp(N)." << endl
         << "\t\t\t\tNon-incremental mode and default mode with a single selected goal." << endl
         << "\t\t-ddrp R" << endl
         << "\t\t\tApplies dd and selects hypothesis that belong to rp(N)" << endl
         << "\t\t\t\tNon-incremental mode and default mode with a single selected goal." << endl
         << "\t\t-h" << endl
         << "\t\t\tPrints help." << endl;
    // TODO minint/maxint
}

static void classifyGoals(const vector<pair<int,int>>& goals, map<int, vector<int>>& sgoals) {
    for(auto& goal: goals) {
        if(sgoals.find(goal.first) == sgoals.end()) {
            sgoals.insert(pair<int,vector<int>>(goal.first, vector<int>()));
        }
        sgoals.at(goal.first).push_back(goal.second);
    }
    for(auto& sgoal: sgoals) {
        std::sort(sgoal.second.begin(),
                  sgoal.second.end(),
                  [](int i, int j) { return i < j; }
        );
    }
}

int main(int argc, char **argv)
{
    std::string input;
    std::string output;
    vector<pair<int, int>> goals;
    bool incr = true;
    bool model = false;
    int rp = -1;
    bool dd = false;

    int arg = 1;
    while (arg < argc) {
        if (strcmp(argv[arg], "-a") == 0) {
            if (arg+2 < argc){
                goals.push_back(std::make_pair(atoi(argv[arg+1]), atoi(argv[arg+2])));
                arg += 3;
            } else {
                display_help();
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[arg], "-i") == 0) {
            if (arg+1 < argc){
                input = argv[arg+1];
                arg += 2;
            } else {
                display_help();
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[arg], "-o") == 0) {
            if (arg+1 < argc){
                output = argv[arg+1];
                arg += 2;
            } else {
                display_help();
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[arg], "-h") == 0) {
            display_help();
            arg += 1;
        } else if (strcmp(argv[arg], "-n") == 0) {
            incr = false;
            arg += 1;
        } else if (strcmp(argv[arg], "-m") == 0) {
            model = true;
            arg += 1;
        } else if (strcmp(argv[arg], "-rp") == 0) {
            if (arg+1 < argc){
                rp = std::stoi(argv[arg+1]); // TODO catch exception and check >= 0
                arg += 2;
            } else {
                display_help();
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[arg], "-ddrp") == 0) {
            if (arg+1 < argc){
                dd = true;
                rp = std::stoi(argv[arg+1]); // TODO catch exception and check >= 0
                arg += 2;
            } else {
                display_help();
                return EXIT_FAILURE;
            }
        } else {
            display_help();
            return EXIT_FAILURE;
        }
    }

    if(input == "" || output == ""){
        display_help();
        return EXIT_FAILURE;
    }

    QFile infile;
    QDomDocument doc;
    infile.setFileName(QString::fromStdString(input));
    if(!infile.exists() || !infile.open(QIODevice::ReadOnly)){
        std::cout << "Error: Cannot open input file.\n";
        exit(EXIT_FAILURE);
    }

    doc.setContent(&infile);
    infile.close();

    pog::Pog pog = pog::read(doc);

    /* The presence of -a parameters indicates that
     * ppTransSmt is used as a writer tool in a proof mechanism:
     * only one file is produced */
    if(0 < goals.size()) {
        if(incr) {
            map<int, vector<int>> sgoals;
            classifyGoals(goals, sgoals);
            ppTransIncr::saveSmtLibFileIncrSome(pog, output, sgoals, model);
        }
        else {
            if (goals.size() != 1) {
                display_help();
                exit(EXIT_FAILURE);
            }
            const int groupId { goals[0].first };
            const int goalId { goals[0].second };
            ppTransNonIncr::saveSmtLibFileNonIncrOne(pog, output, groupId, goalId, rp, dd, model);
        }
    }
    /* ppTransSmt is used as a command-line tool,
     * e.g. to produced benchmarks. */
    else {
        if(incr) {
            ppTransIncr::saveSmtLibFileIncr(pog, output, model);
        }
        else {
            ppTransNonIncr::saveSmtLibFileNonIncr(pog, output, rp, dd, model);
        }
    }
    return EXIT_SUCCESS;
}
