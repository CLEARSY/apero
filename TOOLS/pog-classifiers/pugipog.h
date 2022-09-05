/*
   This file is part of pogmetrics.

   Copyright © CLEARSY 2022

   pogemetrics is free software: you can redistribute it and/or modify
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
   Shortcuts in the pog DOM representation using PUGIXML.

   Copyright © CLEARSY 2022
*/
#include "pugixml.hpp"

namespace pugi {

namespace pog {

/// tests if an expression is an application of the card operator
extern bool is_card(const pugi::xml_node &n);

/// tests if n is an identifier of type INTEGER
extern bool is_integer_symbol(const pugi::xml_node &n);

/// tests if the type of n is set of integers
extern bool is_integer_set(const pugi::xml_node &n);

/// tests if n is an identifier of type BOOL
extern bool is_boolean_symbol(const pugi::xml_node &n);

extern bool is_universal(const pugi::xml_node &n);

extern bool is_existential(const pugi::xml_node &n);

extern bool is_implication(const pugi::xml_node &n);

extern bool is_conjunction(const pugi::xml_node &n);

extern bool is_equality(const pugi::xml_node &n);

} // namespace pog

} // namespace pugi
