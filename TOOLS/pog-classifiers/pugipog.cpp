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

#include <cstring>

#include "pugipog.h"

namespace pugi {
namespace pog {

/// tests if an expression is an application of the card operator
bool is_card(const xml_node &n) {
  return strcmp("Unary_Exp", n.name()) == 0 &&
         strcmp("card", n.attribute("op").value()) == 0;
}

bool is_integer_symbol(const xml_node &n) {
  return strcmp("Id", n.name()) == 0 &&
         strcmp("1", n.attribute("typref").value()) == 0;
}

bool is_integer_set(const xml_node &n) {
  return strcmp("Id", n.name()) == 0 &&
         strcmp("0", n.attribute("typref").value()) == 0;
}

bool is_boolean_symbol(const xml_node &n) {
  return strcmp("Id", n.name()) == 0 &&
         strcmp("5", n.attribute("typref").value()) == 0;
}

bool is_universal(const xml_node &n) {
  return strcmp("Quantified_Pred", n.name()) == 0 &&
         strcmp("!", n.attribute("type").value()) == 0;
}

bool is_existential(const xml_node &n) {
  return strcmp("Quantified_Pred", n.name()) == 0 &&
         strcmp("#", n.attribute("type").value()) == 0;
}

bool is_implication(const xml_node &n) {
  return strcmp("Binary_Pred", n.name()) == 0 &&
         strcmp("=>", n.attribute("op").value()) == 0;
}

bool is_conjunction(const xml_node &n) {
  return strcmp("Nary_Pred", n.name()) == 0 &&
         strcmp("&", n.attribute("op").value()) == 0;
}

bool is_equality(const xml_node &n) {
  return strcmp("Exp_Comparison", n.name()) == 0 &&
         strcmp("=", n.attribute("op").value()) == 0;
}

} // namespace pog

} // namespace pugi
