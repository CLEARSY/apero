/*
   This file is part of pogmetrics.

   Copyright © CLEARSY 2022

   pogmetrics is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/**
   Compute set of free symbols in formula and operation on sets of symbols.

   Quick user notice:

   This is a bare-bones, unefficient implementation.

   Symbols are represented as C++ strings.
   Sets are represented as C++ vectors.
   Membership is tested with a sequential traversal.
   metrics computed.

 */
#ifndef __SYMBOLS_H
#define __SYMBOLS_H

#include<iostream>
#include<string>
#include<vector>

#include "pugixml.hpp"

typedef std::string symbol_t;
typedef std::vector<symbol_t> symbol_set_t;

extern void get_all_symbols(symbol_set_t &symbols, const pugi::xml_node &n);
extern bool contains_symbol(const symbol_set_t& src, const symbol_t& symbol);
extern bool symbol_set_intersects(const symbol_set_t& s1, const symbol_set_t& s2);
extern symbol_set_t& operator+=(symbol_set_t& lhs, const symbol_set_t& rhs);
extern std::ostream& operator<<(std::ostream& os, const symbol_set_t& s);

#endif /* __SYMBOLS_H */
