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

#include<cstring>

#include<algorithm>
#include<iostream>
#include<map>
#include<string>

using std::cerr;
using std::clog;
using std::cout;
using std::endl;
using std::map;
using std::move;
using std::pair;
using std::string;

#include "pugipog.h"
#include "pugixml.hpp"

using namespace pugi;

#include "bounds.h"

#define TRACE_BOUNDS 0

/* -- declarations of local routines -------------------------------------- */

static void eval_bounds_expr(const bounds_table_t &table, const xml_node &expr,
                             bounds_t &bounds);


/* operations on bounds for values of integer constants */
static void bounds_product(bounds_t &result, const bounds_t &lhs,
                           const bounds_t &rhs);
static void bounds_subtract(bounds_t &result, const bounds_t &lhs,
                            const bounds_t &rhs);
static void bounds_sum(bounds_t &result, const bounds_t &lhs,
                       const bounds_t &rhs);

/* operations on bounds for cardinalities of sets */
static void bounds_set_subtract(bounds_t &result, const bounds_t &lhs,
				const bounds_t &rhs);

static void bounds_table_init(bounds_table_t &table);
static bool bounds_table_update(bounds_table_t &table, const string &symbol,
                                const bounds_t &bounds);

#if TRACE_BOUNDS
static void bounds_table_print(const bounds_table_t &table);
#endif

/** maps quantified variables of current quantified formula to bounds */
typedef map<string, bounds_t *> domains_t;

/* ------------------------------------------------------------------------ */

void eval_bounds(const xml_node &root, bounds_table_t &table) {
  bounds_table_init(table);
  for (auto Define : root.children("Define")) {
    string symbol;   // records the symbol likely constrained by the current
                     // hypothesis
    bounds_t bounds; // records the bounds on the symbol defined in the current
                     // hypothesis
    symbol_category_t category; // records the category
    for (auto elem : Define.children()) {
      symbol.clear();
      // set definition
      if (strcmp("Set", elem.name()) == 0) {
        auto Id = elem.first_child();
        category = SET;
        symbol = string(Id.attribute("value").value());
        // an enumerated set
        if (elem.child("Enumerated_Values")) {
          int64_t card = 0;
          const auto &children = elem.child("Enumerated_Values").children("Id");
          for (auto c = children.begin(); c != children.end(); ++c) {
            ++card;
          }
          bounds = {.lbound = card, .ubound = card, .l = true, .u = true};
        } else {
          bounds = {.lbound = 1, .ubound = INT64_MAX, .l = true, .u = false};
        }
      }
      // an integer symbol is compared with another expression
      // patterns recognized and handled are the following
      // c : l..u
      // c : NAT
      // c = card(s)
      // c = expr
      else if (strcmp("Exp_Comparison", elem.name()) == 0 &&
               pog::is_integer_symbol(elem.first_child())) {
        xml_node lhs{elem.first_child()};
        xml_node rhs{lhs.next_sibling()};
        const char *op = elem.attribute("op").value();
        // c : NAT, c : l..u
        if (strcmp(":", op) == 0) {
          category = INTEGER_CONSTANT;
          symbol = string(lhs.attribute("value").value());
          eval_bounds_expr(table, rhs, bounds);
        }
        // c = card(s)
        else if (strcmp("=", op) == 0 && pog::is_card(rhs) &&
                 strcmp("Id", rhs.first_child().name()) == 0) {
          category = SET;
          symbol = string(rhs.first_child().attribute("value").value());
          eval_bounds_expr(table, lhs, bounds);
        }
        // c = expr
        else if (strcmp("=", op) == 0 && pog::is_integer_symbol(lhs)) {
          category = INTEGER_CONSTANT;
          symbol = string(lhs.attribute("value").value());
          eval_bounds_expr(table, rhs, bounds);
        } else {
          continue;
        }
      }
      // an integer set is compared with another expression
      // patterns recognized and handled are the following
      // c = expr
      else if (pog::is_equality(elem) &&
               pog::is_integer_set(elem.first_child())) {
        category = SET;
        symbol = string(elem.first_child().attribute("value").value());
        eval_bounds_expr(table, elem.first_child().next_sibling(), bounds);
      } else {
        continue;
      }
      if (symbol.empty()) {
        cerr << "Internal error: attempting to set bounds on an unidentified "
                "symbol"
             << endl;
      }
      bool updated = bounds_table_update(table, symbol, bounds);
      if (updated) {
        continue;
      }
      symbol_bounds_t *p = new symbol_bounds_t;
      p->category = category;
      p->bounds = bounds;
      table.insert(move(pair<string, symbol_bounds_t *>(symbol, p)));
    }
  }
#if TRACE_BOUNDS
  bounds_table_print(table);
#endif
}


static void eval_bounds_expr(const bounds_table_t &table, const xml_node &expr,
                             bounds_t &bounds) {
  if (strcmp("Integer_Literal", expr.name()) == 0) {
    int64_t v = strtol(expr.attribute("value").value(), nullptr, 10);
    if (errno == ERANGE) {
      cerr << "Integer literal " << expr.attribute("value").value()
	   << " cannot be used as a bound." << endl;
      return;
    }
    bounds.l = true;
    bounds.u = true;
    bounds.lbound = v;
    bounds.ubound = v;
  } else if (strcmp("Id", expr.name()) == 0) {
    string name(expr.attribute("value").value());
    auto p = table.find(name);
    if (p != table.end()) {
      symbol_bounds_t *info = p->second;
      bounds = info->bounds;
    }
  } else if (strcmp("Binary_Exp", expr.name()) == 0 &&
             strcmp("..", expr.attribute("op").value()) == 0) {
    auto low = expr.first_child();
    auto upp = low.next_sibling();
    bounds_t blow;
    bounds_t bupp;
    eval_bounds_expr(table, low, blow);
    eval_bounds_expr(table, upp, bupp);
    if (blow.l) {
      bounds.l = true;
      bounds.lbound = blow.lbound;
    }
    if (bupp.l) {
      bounds.u = true;
      bounds.ubound = bupp.ubound;
    }
  } else if (strcmp("Binary_Exp", expr.name()) == 0 &&
             strcmp("*i", expr.attribute("op").value()) == 0) {
    auto low = expr.first_child();
    auto upp = low.next_sibling();
    bounds_t lhs;
    bounds_t rhs;
    eval_bounds_expr(table, low, lhs);
    eval_bounds_expr(table, upp, rhs);
    bounds_product(bounds, lhs, rhs);
  } else if (strcmp("Binary_Exp", expr.name()) == 0 &&
             strcmp("-i", expr.attribute("op").value()) == 0) {
    auto low = expr.first_child();
    auto upp = low.next_sibling();
    bounds_t lhs;
    bounds_t rhs;
    eval_bounds_expr(table, low, lhs);
    eval_bounds_expr(table, upp, rhs);
    bounds_subtract(bounds, lhs, rhs);
  } else if (strcmp("Binary_Exp", expr.name()) == 0 &&
             strcmp("+i", expr.attribute("op").value()) == 0) {
    auto low = expr.first_child();
    auto upp = low.next_sibling();
    bounds_t lhs;
    bounds_t rhs;
    eval_bounds_expr(table, low, lhs);
    eval_bounds_expr(table, upp, rhs);
    bounds_sum(bounds, lhs, rhs);
    
  }
  /* set subtraction card(s1 - s2) */
  else if (strcmp("Binary_Exp", expr.name()) == 0 &&
	   strcmp("-s", expr.attribute("op").value()) == 0) {
    auto s1 = expr.first_child();
    auto s2 = s1.next_sibling();
    bounds_t lhs;
    bounds_t rhs;
    eval_bounds_expr(table, s1, lhs);
    eval_bounds_expr(table, s2, rhs);
    bounds_set_subtract(bounds, lhs, rhs);
  } else {
    cerr << "Case " << expr.name() << " " << expr.attribute("op").value()
	 << " not handled" << endl;
  }
}

/* ------------------------------------------------ operations on bounds -- */

static void bounds_product(bounds_t &result, const bounds_t &lhs,
                           const bounds_t &rhs) {
  if (lhs.l && rhs.l && 0 <= lhs.lbound && 0 <= rhs.lbound) {
    result.l = true;
    result.lbound = lhs.lbound * rhs.lbound;
    if (lhs.u && rhs.u) {
      result.u = true;
      result.ubound = lhs.ubound * rhs.ubound;
    } else {
      cerr << "Upper bound of bounds product not computed" << endl;
    }
  } else {
    cerr << "Bounds product not computed" << endl;
  }
}

static void bounds_subtract(bounds_t &result, const bounds_t &lhs,
                            const bounds_t &rhs) {
  if (lhs.l && rhs.u) {
    result.l = true;
    result.lbound = lhs.lbound - rhs.ubound;
  }
  if (lhs.u && rhs.l) {
    result.u = true;
    result.ubound = lhs.ubound - rhs.lbound;
  }
}

static void bounds_sum(bounds_t &result, const bounds_t &lhs,
                       const bounds_t &rhs) {
  if (lhs.l && rhs.l) {
    result.l = true;
    result.lbound = lhs.lbound + rhs.ubound;
  }
  if (lhs.u && rhs.u) {
    result.u = true;
    result.ubound = lhs.ubound + rhs.lbound;
  }
}


static void bounds_set_subtract(bounds_t &result, const bounds_t &lhs,
				const bounds_t &rhs) {
  if (lhs.u) {
    result.u = true;
    result.ubound = lhs.ubound;
  }
  result.l = true;
  result.lbound = 0;
  if (lhs.l && rhs.u && rhs.ubound < lhs.lbound) {
    result.lbound = lhs.lbound - rhs.ubound;
  }
}

/**
   \brief tightens bounds information, when applicable, with given input

   \param target bounds information to be tightened
   \param info tightening bounds info
*/
void update_bounds(bounds_t *target, const bounds_t &info) {
  if (info.l) {
    /// tighten an existing lower bound
    if (target->l && target->lbound < info.lbound) {
      target->lbound = info.lbound;
    }
    /// set a new lower bound
    else if (!target->l) {
      target->l = true;
      target->lbound = info.lbound;
    }
  }
  if (info.u) {
    /// tighten an existing upper bound
    if (target->u && info.ubound < target->ubound) {
      target->ubound = info.ubound;
    }
    /// sets a new upper bound
    else if (!target->u) {
      target->u = true;
      target->ubound = info.ubound;
    }
  }
}
/* -------------------------------------------------------- bounds_table -- */

static void bounds_table_init(bounds_table_t &table) {
  string symbol;
  symbol.assign("MAXINT");
  {
    symbol_bounds_t *p = new symbol_bounds_t;
    p->category = INTEGER_CONSTANT;
    p->bounds = {
        .lbound = INT32_MAX, .ubound = INT32_MAX, .l = true, .u = true};
    table.insert(std::move(pair<string, symbol_bounds_t *>(symbol, p)));
  }
  symbol.assign("MININT");
  {
    symbol_bounds_t *p = new symbol_bounds_t;
    p->category = INTEGER_CONSTANT;
    p->bounds = {
        .lbound = INT32_MIN, .ubound = INT32_MIN, .l = true, .u = true};
    table.insert(std::move(pair<string, symbol_bounds_t *>(symbol, p)));
  }
  symbol.assign("INT");
  {
    symbol_bounds_t *p = new symbol_bounds_t;
    p->category = SET;
    p->bounds = {
        .lbound = INT32_MIN, .ubound = INT32_MAX, .l = true, .u = true};
    table.insert(std::move(pair<string, symbol_bounds_t *>(symbol, p)));
  }
  symbol.assign("NAT");
  {
    symbol_bounds_t *p = new symbol_bounds_t;
    p->category = SET;
    p->bounds = {.lbound = 0, .ubound = INT32_MAX, .l = true, .u = true};
    table.insert(std::move(pair<string, symbol_bounds_t *>(symbol, p)));
  }
  symbol.assign("NAT1");
  {
    symbol_bounds_t *p = new symbol_bounds_t;
    p->category = SET;
    p->bounds = {.lbound = 1, .ubound = INT32_MAX, .l = true, .u = true};
    table.insert(std::move(pair<string, symbol_bounds_t *>(symbol, p)));
  }
  symbol.assign("BOOL"); // hack
  {
    symbol_bounds_t *p = new symbol_bounds_t;
    p->category = SET;
    p->bounds = {.lbound = 0, .ubound = 1, .l = true, .u = true};
    table.insert(std::move(pair<string, symbol_bounds_t *>(symbol, p)));
  }
}

static bool bounds_table_update(bounds_table_t &table, const string &symbol,
                                const bounds_t &bounds) {
  auto p = table.find(symbol);
  if (p == table.end())
    return false;
  update_bounds(&p->second->bounds, bounds);
  return true;
}


#if TRACE_BOUNDS
static void bounds_table_print(const bounds_table_t &table) {
  for (auto p : table) {
    string name = p.first;
    symbol_bounds_t *info = p.second;
    switch (info->category) {
    case INTEGER_CONSTANT: {
      clog << "INTEGER_CONSTANT: " << name << " : " << info->bounds.l << " "
           << info->bounds.u << " " << info->bounds.lbound << " "
           << info->bounds.ubound << endl;
      break;
    }
    case SET: {
      clog << "SET: " << name << " : " << info->bounds.l << " "
           << info->bounds.u << " " << info->bounds.lbound << ".."
           << info->bounds.ubound << endl;
      break;
    }
    }
  }
}
#endif

