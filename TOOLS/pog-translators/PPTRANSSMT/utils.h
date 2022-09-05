/** utils.h

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
#ifndef UTILS_H
#define UTILS_H

#include<string>

namespace utils {

using std::string;

/**
 * @brief absoluteBasename returns the full absolute path minus the file extension
 * @param filename a path in the current file system
 * @return the absolute path corresponding to the given path, with the file extension
 * removed.
 */
extern string absoluteBasename(const string& filename);

/**
 * @brief absoluteName returns the full absolute path up to and including the file extension
 * @param filename a path in the current file system
 * @return the absolute path corresponding to the given path.
 */
extern string absoluteFilePath(const string& filename);

}

#endif // UTILS_H
