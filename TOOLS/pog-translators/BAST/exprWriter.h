/** exprWriter.h

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
#ifndef EXPRWRITER_H
#define EXPRWRITER_H

#include "expr.h"
#include <QXmlStreamWriter>

namespace Xml {
    void writeTypedVar(QXmlStreamWriter &stream, std::map<BType,unsigned int> &typeInfos, const TypedVar &v);
    void writeExpression(QXmlStreamWriter &stream, std::map<BType,unsigned int> &typeInfos, const Expr &p);
}

#endif // EXPRWRITER_H
