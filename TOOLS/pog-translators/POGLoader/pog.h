/** pog.h

   \copyright Copyright © CLEARSY 2022
   \license This file is part of POGLoader.

   POGLoader is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

    POGLoader is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with POGLoader. If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef POG_H
#define POG_H

#include <vector>
#include <string>
#include <QDomDocument>

#include "pred.h"
#include "gpred.h"

namespace pog {

    class PogException : public std::exception
    {
        public:
            PogException(const std::string desc):description{desc}{};
            ~PogException() throw(){};
            const char *what() const throw(){ return description.c_str(); };
        private:
            std::string description;
    };

    class PO {
        public:
            std::string tag;
            std::vector<int> localHypsRef;
            Pred goal;
            PO( const std::string &tag,
                    const std::vector<int> &localHypsRef,
                    Pred &&goal)
                : tag{tag},
                localHypsRef{localHypsRef},
                goal{std::move(goal)}
            {}
            PO copy() const {
                return PO(tag,localHypsRef,goal.copy());
            }
    };

    class POGroup {
        public:
            std::string tag;
            size_t goalHash;
            std::vector<std::string> definitions;
            std::vector<Pred> hyps; // chaque element d'une conjonction est stocké séparement
            std::vector<Pred> localHyps; // chaque element d'une conjonction est stocké séparement
            std::vector<PO> simpleGoals;
            POGroup(const std::string &tag,
                    size_t goalHash,
                    const std::vector<std::string> &definitions,
                    std::vector<Pred> &&hyps,
                    std::vector<Pred> &&localHyps,
                    std::vector<PO> &&simpleGoals):
                tag{tag},definitions{definitions},hyps{std::move(hyps)},
                localHyps{std::move(localHyps)},simpleGoals{std::move(simpleGoals)},goalHash{goalHash} {};
    };

    class Set {
        public:
            Set(const TypedVar &setName,const std::vector<TypedVar> &elts):
                setName{setName},elts{elts}{};
            TypedVar setName;
            std::vector<TypedVar> elts;

            static int compare(const Set &v1, const Set& v2);
            inline bool operator==(const Set& other) const { return compare(*this,other) == 0; }
            inline bool operator!=(const Set& other) const { return compare(*this,other) != 0; }
            inline bool operator< (const Set& other) const { return compare(*this,other) <  0; }
            inline bool operator> (const Set& other) const { return compare(*this,other) >  0; }
            inline bool operator<=(const Set& other) const { return compare(*this,other) <= 0; }
            inline bool operator>=(const Set& other) const { return compare(*this,other) >= 0; }
    };

    class Define {
        public:
            Define(const std::string &name, size_t hash=0):name{name},hash{hash}{};
            std::string name;
            size_t hash;
            std::vector<Set> gsets;
            std::vector<Pred> ghyps; // chaque element d'une conjonction est stocké séparement
    };

    class Pog {
        public:
            std::vector<Define> defines;
            std::vector<POGroup> pos;
    };

  struct Options {
        QStringList ressources;
        bool generateWDPOs = false;
        bool overflow = false;
        bool obvious = false;
        bool verbose = true;
        int limit = 10000;
    };

    Pog read(const QDomDocument &pog);
}

#endif // POG_H
