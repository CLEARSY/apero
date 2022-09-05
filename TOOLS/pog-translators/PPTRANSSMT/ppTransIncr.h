/** ppTransIncr.h

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
#ifndef PPTRANSINCR_H
#define PPTRANSINCR_H

#include<map>
#include<string>
#include<vector>

#include "pog.h"

namespace ppTransIncr {

using std::map;
using std::string;
using std::vector;

/**
 * @brief saveSmtLibFileIncrSome saves the translation to incremental SMT of some goals
 * @param pog The source POG file contents from which goals are selected
 * @param filename The target SMT-LIB2 file path
 * @param goals The selected goals
 * @param model
 * @param minint
 * @param maxint
 */
void saveSmtLibFileIncrSome(pog::Pog &pog,
                            const string &filename,
                            map<int, vector<int>>& goals,
                            bool model,
                            const string &minint = "(- 2147483648)",
                            const string &maxint = "2147483647");

/**
 * @brief saveSmtLibFileIncr saves the translation to incremental SMT
 * @param pog The source POG file contents
 * @param filename The target SMT-LIB2 file path
 * @param model
 * @param minint
 * @param maxint
 */
void saveSmtLibFileIncr(pog::Pog &pog,
                        const string &filename,
                        bool model,
                        const string &minint = "(- 2147483648)",
                        const string &maxint = "2147483647");
}

#endif // PPTRANSINCR_H
