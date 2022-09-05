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

#include<cassert>
#include<cstring>
#include<string>

#include "symbols.h"

using namespace pugi;
using std::string;

/* (yes this design is computationally suboptimal) */

bool contains_symbol(const symbol_set_t& src, const symbol_t& symbol) {
  for (size_t i = 0; i < src.size(); i++)
    if (src[i].compare(symbol) == 0)
      return true;
  return false;
}

bool symbol_set_intersects(const symbol_set_t& s1, const symbol_set_t& s2) {
  for (const auto& symbol: s1) {
    if (contains_symbol(s2, symbol)) {
      return true;
    }
  }
  return false;
}


symbol_set_t& operator+=(symbol_set_t& lhs, const symbol_set_t& rhs) {
  for (const auto s: rhs) {
    if (!contains_symbol(lhs, s)) {
      lhs.push_back(s);
    }
  }
  return lhs;
}

std::ostream& operator<<(std::ostream& os, const symbol_set_t& s) {
  bool first = true;
  os << '{';
  for(auto ptr = s.begin(); ptr != s.end(); ++ptr) {
    if (!first) {
      os << ',';
    }
    else {
      first = false;
    }
    os << *ptr;
  }
  os << '}';
  return os;
}

static void get_all_symbols_rec(symbol_set_t &result, const xml_node &n,
                                symbol_set_t &bound_symbols);

void get_all_symbols(symbol_set_t &symbols, const xml_node &n) {
  symbol_set_t bound_symbols;
  get_all_symbols_rec(symbols, n, bound_symbols);
  assert(bound_symbols.empty());
}

static void get_all_symbols_rec(symbol_set_t &result, const xml_node &n,
                                symbol_set_t &bound_symbols) {
  const char *tag = n.name();

  if (strcmp("Binary_Exp", tag) == 0 || strcmp("Exp_Comparison", tag) == 0 ||
      strcmp("Nary_Exp", tag) == 0 || strcmp("Binary_Pred", tag) == 0 ||
      strcmp("Nary_Pred", tag) == 0 || strcmp("Ternary_Pred", tag) == 0 ||
      strcmp("Unary_Exp", tag) == 0 || strcmp("Unary_Pred", tag) == 0) {
    // recurse over descendants
    for (const auto child : n.children()) {
      get_all_symbols_rec(result, child, bound_symbols);
    }
  } else if (strcmp("Boolean_Exp", tag) == 0) {
    get_all_symbols_rec(result, n.first_child(), bound_symbols);
  } else if (strcmp("Quantified_Exp", tag) == 0 ||
             strcmp("Quantified_Pred", tag) == 0 ||
             strcmp("Quantified_Set", tag) == 0) {
    // recurse over descendants, but not the first (list of variables)
    int nb_vars = 0;
    for (const auto var : n.child("Variables").children()) {
      nb_vars++;
      string op(var.attribute("value").value());
      bound_symbols.push_back(op);
    }
    get_all_symbols_rec(result, n.child("Pred").first_child(), bound_symbols);
    get_all_symbols_rec(result, n.child("Body").first_child(), bound_symbols);
    while (nb_vars > 0) {
      bound_symbols.pop_back();
      nb_vars--;
    }
  } else if (strcmp("Id", tag) == 0) {
    symbol_t s {string(n.attribute("value").value())};
    if (!contains_symbol(result, s) && !contains_symbol(bound_symbols, s)) {
      result.push_back(s);
    }
    return;
  }
}
