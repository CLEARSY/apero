/** hashutil.h

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
#ifndef HASHUTIL_H
#define HASHUTIL_H

#include<string>

namespace hashUtil {
    // These functions are specialisations of boost::hash_combine
    inline size_t hash_combine_int(int i, size_t seed){
        seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }

    inline size_t hash_combine_string(const std::string &s, size_t seed){
        std::hash<std::string> h;
        seed ^= h(s) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
}

#endif // HASHUTIL_H
