/** ppTrans.h

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
#ifndef PPTRANS_H
#define PPTRANS_H

#include<map>
#include<set>
#include<vector>
#include <sstream>
#include "pog.h"

class ppTransException : public std::exception
{
public:
    ppTransException(const std::string &desc);
    ~ppTransException() throw();
    const char *what() const throw();
private:
    std::string description;
};

namespace ppTrans {
    class LocalEquations;
    class Context {
        public:
            void pop_vars();
            void push_vars(const std::vector<TypedVar> &vars);
            std::string registerId(const VarName &v, const BType &ty, std::set<std::string> &used_ids);
            std::string registerMem(const BType &ty, std::set<std::string> &used_ids);
            std::string registerIterate(const BType &ty, std::set<std::string> &used_ids);
            std::string registerRecordType(const BType &ty, std::set<std::string> &used_ids);
            std::string registerStringLiteral(const std::string &s, std::set<std::string> &used_ids);
            std::string nameSimpleExpression(const Expr &e, LocalEquations &local_eqs, std::set<std::string> &used_ids);
            const std::map<std::string,std::string> &getSmtLibDeclarations() const { return smtLibDeclarations; } ;
        private:
            std::map<BType,std::string> memberships = { {BType::POW_INT,"mem0"}, {BType::POW_REAL,"mem1"} };
            std::map<BType,std::string> recordTypes; 
            std::map<BType,std::string> iterates; 
            std::map<std::string,std::string> stringLiterals; 
            std::map<TypedVar,std::string> globalIdents;
            std::vector<std::vector<TypedVar>> bv_stack;
            std::map<std::string,std::string> smtLibDeclarations = {};
    };
    void ppTrans(std::ostringstream &str, Context &ctx, const Pred &p, std::set<std::string> &used_ids);
    void ppTrans(std::ostringstream &str, Context &env, const pog::Set &set,std::set<std::string> &used_ids);
    void printPrelude ( std::ofstream &out, const std::string &minint, const std::string &maxint );
}

#endif // PPTRANS_H
