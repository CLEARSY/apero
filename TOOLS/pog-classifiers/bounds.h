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
   Rudimentary static analysis to bound integer values and set cardinalities.

   Call `eval_bounds` to evaluate conservatively bounds on
   symbols. Currently this only provides bound information for
   sets (their cardinality) and integer constants (their value).
   See types `bounds_t` and `symbol_bounds_t`.
   The result is stored in the `bounds_table_t` passed as a
   parameter.

 */
#ifndef __BOUNDS_H
#define __BOUNDS_H

#include<iostream>
#include<map>
#include<string>

#include "pugixml.hpp"

/// (B) logic symbol category
enum symbol_category_t {
			SET = 0,             /// a set (basic or enumerated)
			INTEGER_CONSTANT = 1 /// a constant (base type is integer)
};

/// bounds information
typedef struct bounds_t {
  int64_t lbound = INT64_MIN; /// lower bound, if any
  int64_t ubound = INT64_MAX; /// upper bound, if any
  bool l = false;             /// lower bound exists (true) or not (false)
  bool u = false;             /// upper bound exists (true) or not (false)
} bounds_t;

/// logic symbol information
typedef struct symbol_bounds_t {
  symbol_category_t category;
  bounds_t bounds; // on: values for INTEGER_CONSTANT, cardinality for SET
} symbol_bounds_t;

/// a data type for symbol tables containing bounds information
typedef std::map<std::string, symbol_bounds_t *> bounds_table_t;


/** bounds possible values of integer constants and cardinalities of sets
    \param root the dom of a POG file
    \param table the bounds infered are stored there
    \note The routine infers bounds for the following constructions
    - enumerated sets
    - interval membership c : l..u (bounds the value of integer constant c)
    - set membershup c : NAT (bounds the value of integer constant c)
    - c = card(s) (bounds cardinality of s from the value of c)
    - c = expr (bounds the value of integer constant c)
      where expr is
      - an integer literal
      - an integer constant
      - expr * expr (only on some cases, depending on the signs of expr)
      - expr - expr
      - expr + expr
    - s = expr (bounds the cardinality of set s)
      where expr is
      - a set
      - expr..expr
      - expr - expr (set difference)
*/
extern void eval_bounds(const pugi::xml_node &root, bounds_table_t &table);

/** update given bound with new bound information 
    \param target bound to be updated (tightened)
    \param info new bound information
*/
extern void update_bounds(bounds_t *target, const bounds_t &info);

#endif /* __BOUNDS_H */
