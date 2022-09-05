/** exprReader.h

   \copyright Copyright © CLEARSY 2022
   \license This file is part of BAST.

   BAST is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

    BAST is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BAST. If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef EXPRREADER_H
#define EXPRREADER_H

#include "expr.h"
#include<QDomElement>

namespace Xml {
    TypedVar VarNameFromId(const QDomElement &id, const std::vector<BType> &typeInfos);

    class ExprReaderException : public std::exception
    {
        public:
            ExprReaderException(const std::string desc, int line):
                description{desc + " (line " + std::to_string(line) + ")"}
            {};
            ~ExprReaderException() throw(){};
            const char *what() const throw(){ return description.c_str(); };
        private:
            std::string description;
    };
    Expr readExpression(const QDomElement &dom, const std::vector<BType> &typeInfos);
}

#endif // EXPRREADER_H
