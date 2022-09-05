/*
  This file is part of pogmetrics.

  Copyright © CLEARSY 2022

  This program is free software: you can redistribute it and/or modify
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

#include<algorithm>
#include<array>
#include<iostream>
#include<map>
#include<string>

using std::cerr;
using std::clog;
using std::cout;
using std::endl;
using std::array;
using std::map;
using std::move;
using std::pair;
using std::string;

#include "pugipog.h"
#include "pugixml.hpp"

using namespace pugi;

#include "metrics.h"
#include "options.h"

/* -- declarations of local routines -------------------------------------- */

/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------ hyco_metrics -- */

static hyco_metrics_t& operator+=(hyco_metrics_t &lhs, const hyco_metrics_t& rhs) {
  lhs.global += rhs.global;
  lhs.local += rhs.local;

  return lhs;
}

static void hyco_metrics_header() {
  std::cout << "global" << g_program_options.delimiter << "local";
}

static std::ostream &operator<<(std::ostream &os, const hyco_metrics_t &rhs) {
  char d = g_program_options.delimiter;
  os << rhs.global << d << rhs.local;
  return os;
}

static bool is_null(const hyco_metrics_t& m) {
  return 0 == m.global && 0 == m.local;
}

/* ---------------------------------------------------- opco_metrics_t -- */

/** Set of operators name */
const array v_operators_name{
  /* source : pog.xsd */
  // binary_pred_op
  string("=>"), string("<=>"),
    // comparison_op
    string(":"), string("/:"), string("<:"), string("/<:"), string("<<:"),
    string("/<<:"), string("="), string("/="), string(">=i"), string(">i"),
    string("<i"), string("<=i"), string(">=r"), string(">r"), string("<r"),
    string("<=r"), string(">=f"), string(">f"), string("<f"), string("<=f"),
    // quantified_pred_op
    string("!"), string("#"),
    // unary_pred_op
    string("not"),
    // nary_pred_op
    string("&"), string("or"),
    // unary_exp_op
    // binary_exp_op
    string("),"), string("*"), string("*i"), string("*r"), string("*f"),
    string("*s"), string("**"), string("**i"), string("**r"), string("+"),
    string("+i"), string("+r"), string("+f"), string("+->"), string("+->>"),
    string("-"), string("-i"), string("-r"), string("-f"), string("-s"),
    string("-->"), string("-->>"), string("->"), string(".."), string("/"),
    string("/i"), string("/r"), string("/f"), string("/\\"), string("/|\\"),
    string(";"), string("<+"), string("<->"), string("<-"), string("<<|"),
    string("<|"), string(">+>"), string(">->"), string(">+>>"), string(">->>"),
    string("><"), string("||"), string("\\/"), string("\\|/"), string("^"),
    string("mod"), string("|->"), string("|>"), string("|>>"), string("["),
    string("("), string("<'"), string("prj1"), string("prj2"),
    string("iterate"), string("const"), string("rank"), string("father"),
    string("subtree"), string("arity"),
    // ternary_exp_op
    string("son"), string("bin"),
    // nary_exp_op
    string("["), string("{"),
    // quantified_exp_op
    string("%"), string("SIGMA"), string("iSIGMA"), string("rSIGMA"),
    string("PI"), string("iPI"), string("rPI"), string("INTER"),
    string("UNION"), string("max"), string("imax"), string("rmax"),
    string("min"), string("imin"), string("rmin"), string("card"),
    string("dom"), string("ran"), string("POW"), string("POW1"), string("FIN"),
    string("FIN1"), string("union"), string("inter"), string("seq"),
    string("seq1"), string("iseq"), string("iseq1"), string("-"), string("-i"),
    string("-r"), string("~"), string("size"), string("perm"), string("first"),
    string("last"), string("id"), string("closure"), string("closure1"),
    string("tail"), string("front"), string("rev"), string("conc"),
    string("succ"), string("pred"), string("rel"), string("fnc"),
    string("real"), string("floor"), string("ceiling"), string("tree"),
    string("btree"), string("top"), string("sons"), string("prefix"),
    string("postfix"), string("sizet"), string("mirror"), string("left"),
    string("right"), string("infix"), string("bin"), string("Quantified_Set"),
    string("Boolean_Literal"), string("Integer_Literal"),
    string("Real_Literal"), string("EmptySet"), string("EmptySeq"),
    string("Boolean_Exp"), string("Set"),
    // reserved for unidentified expressions
    string("UNIDENTIFIED")};

/*
  There are C_number_operators different operators.
*/
const auto C_number_operators = v_operators_name.size();

void opco_metrics_formula(opco_metrics_t &metrics, xml_node n) {
  string op;
  if (strcmp("Binary_Exp", n.name()) == 0 ||
      strcmp("Exp_Comparison", n.name()) == 0 ||
      strcmp("Nary_Exp", n.name()) == 0 ||
      strcmp("Binary_Pred", n.name()) == 0 ||
      strcmp("Nary_Pred", n.name()) == 0 ||
      strcmp("Ternary_Pred", n.name()) == 0 ||
      strcmp("Unary_Exp", n.name()) == 0 ||
      strcmp("Unary_Pred", n.name()) == 0) {
    op = n.attribute("op").value();
    for (const auto c : n.children()) {
      opco_metrics_formula(metrics, c);
    }
  }
  else if (strcmp("Boolean_Exp", n.name()) == 0) {
    op = string("Boolean_Exp");
    opco_metrics_formula(metrics, n.first_child());
  }
  else if (strcmp("Quantified_Exp", n.name()) == 0 ||
	   strcmp("Quantified_Pred", n.name()) == 0) {
    op = n.attribute("type").value();
    opco_metrics_formula(metrics, n.child("Pred").first_child());
    opco_metrics_formula(metrics, n.child("Body").first_child());
  }
  else if (strcmp("Quantified_Set", n.name()) == 0) {
    op = string("Quantified_Set");
    opco_metrics_formula(metrics, n.child("Pred").first_child());
    opco_metrics_formula(metrics, n.child("Body").first_child());
  }
  else if (strcmp("Boolean_Literal", n.name()) == 0 ||
	   strcmp("Integer_Literal", n.name()) == 0 ||
	   strcmp("Real_Literal", n.name()) == 0 ||
	   strcmp("String_Literal", n.name()) == 0 ||
	   strcmp("EmptySeq", n.name()) == 0 ||
	   strcmp("EmptySet", n.name()) == 0 ||
	   strcmp("Set", n.name()) == 0) {
    op = string(n.name());
  } else if (strcmp("Id", n.name()) == 0) {
    return;
  } else {
    op = string("UNIDENTIFIED");
  }
  // increment counter
  const auto ptr {metrics.counters.find(op)};
  if (ptr == metrics.counters.end()) {
    cerr << "Error: internal data structures are corrupted." << endl;
    return;
  }
  ++ptr->second;
  metrics.null = false;
}

static std::ostream& operator<<(std::ostream& os, const opco_metrics_t &metrics);
static opco_metrics_t& operator+=(opco_metrics_t &lhs, const opco_metrics_t& rhs) {
  assert(lhs.counters.size() == rhs.counters.size());
  if (!rhs.null) {
    if (lhs.null) {
      auto plhs = lhs.counters.begin();
      auto prhs = rhs.counters.begin();
      while (plhs != lhs.counters.end() && prhs != rhs.counters.end()) {
	assert(plhs->first.compare(prhs->first) == 0);
	plhs->second = prhs->second;
	++plhs;
	++prhs;
      }
      lhs.null = false;
    }
    else {
      auto plhs = lhs.counters.begin();
      auto prhs = rhs.counters.begin();
      while (plhs != lhs.counters.end() && prhs != rhs.counters.end()) {
	assert(plhs->first.compare(prhs->first) == 0);
	plhs->second += prhs->second;
	++plhs;
	++prhs;
      }
    }
  }
  return lhs;
}

static void opco_metrics_header(void) {
  for (auto i = 0u; i < C_number_operators; ++i) {
    if (0 < i) {
      std::cout << g_program_options.delimiter;
    }
    std::cout << v_operators_name[i];
  }
}

static std::ostream& operator<<(std::ostream& os, const opco_metrics_t &metrics) {
  char d = g_program_options.delimiter;
  for (auto i = 0u; i < C_number_operators; ++i) {
    if (0 < i)
      os << d;
    const auto ptr {metrics.counters.find(v_operators_name[i])};
    if (ptr == metrics.counters.end()) {
      cerr << "Error: internal data structures are corrupted." << endl;
      continue;
    }
    os << ptr->second;
  }
  return os;
}

static bool is_null(const opco_metrics_t& m) {
  return m.null;
}

/* ---------------------------------------------------- qudo_metrics_t -- */

/** maps quantified variables of current quantified formula to bounds */

typedef map<string, bounds_t *> domains_t;

static void qudo_metrics_guard(const bounds_table_t &table,
                               const xml_node &guard,
			       domains_t &domains) {
  if (pog::is_conjunction(guard)) {
    for (const auto &g : guard.children()) {
      qudo_metrics_guard(table, g, domains);
    }
    return;
  }
  if (strcmp("Exp_Comparison", guard.name()) == 0 &&
      strcmp(":", guard.attribute("op").value()) == 0) {
    auto lhs = guard.first_child();
    auto rhs = lhs.next_sibling();
    if (!pog::is_integer_symbol(guard.first_child()))
      return;
    // v : s
    if (pog::is_integer_set(rhs)) {
      string v(lhs.attribute("value").value());
      auto itr = domains.find(v);
      if (itr != domains.end()) {
        string s(rhs.attribute("value").value());
        auto sitr = table.find(s);
        if (sitr != table.end()) {
          update_bounds(itr->second, sitr->second->bounds);
        }
      }
    }
    return;
  }
}

static void qudo_metrics_quantified(const bounds_table_t &table,
                                    const xml_node &quantified,
                                    qudo_metrics_t& metrics) {
  // the domain is only evaluated for quantified variable that have an integer
  // base type
  // TODO generalize for types that are not higher order
  // first-order type ::= integer | cartesian_product(first-order type,
  // first-order type)
  domains_t domains;
  for (auto id : quantified.child("Variables").children("Id")) {
    if (pog::is_integer_symbol(id)) {
      bounds_t *bounds = new bounds_t;
      domains.insert(move(
			  pair<string, bounds_t *>(id.attribute("value").value(), bounds)));
    } else {
      cerr << "higher-order quantification not handled" << endl;
    }
  }
  /*
    We only want to deal with the following patterns for now:
        <Quantified_Pred type="!">
            <Variables>...</Variables>
            <Body>
                <Binary_Pred op="=&gt;">
                    <Nary_Pred op="&amp;">
                       [[[ GUARD ]]]
                    </Nary_Pred>
                    ...
                </Binary_Pred>
            </Body>
        </Quantified_Pred>
        <Quantified_Pred type="#">
            <Variables>...</Variables>
            <Body>
                <Nary_Pred op="&amp;">
                    <Nary_Pred op="&amp;">
                       [[[ GUARD ]]]
                    </Nary_Pred>
                    ...
                </Binary_Pred>
            </Body>
        </Quantified_Pred>
  */
  xml_node body = quantified.child("Body").first_child();
  if ((pog::is_universal(quantified) && pog::is_implication(body) &&
       pog::is_conjunction(body.first_child())) ||
      (pog::is_existential(quantified) && pog::is_conjunction(body) &&
       pog::is_conjunction(body.first_child()))) {
    auto guard = body.first_child();
    qudo_metrics_guard(table, guard, domains);
  } else {
    cerr << "quantified formula pattern not handled" << endl;
  }
  for (auto id : quantified.child("Variables").children("Id")) {
    string v(id.attribute("value").value());
    const auto &itr = domains.find(v);
    if (itr != domains.end()) {
      bounds_t *b = itr->second;
      if (!b->l || !b->u) {
        ++metrics.unbounded;
      } else {
        if (b->ubound <= 10) {
          ++metrics.le10;
        } else if (b->ubound <= 100) {
          ++metrics.le100;
        } else {
          ++metrics.gt100;
        }
      }
    } else {
      ++metrics.unknown;
    }
  }
  qudo_metrics_formula(table, body, metrics);
}

void qudo_metrics_formula(const bounds_table_t &table, const xml_node f,
			  qudo_metrics_t& metrics) {
  const char *tag = f.name();
  if (strcmp("Quantified_Pred", tag) == 0) {
    qudo_metrics_quantified(table, f, metrics);
  } else {
    for (const auto &c : f.children()) {
      qudo_metrics_formula(table, c, metrics);
    }
  }
}

static qudo_metrics_t& operator+=(qudo_metrics_t &lhs, const qudo_metrics_t& rhs) {
  lhs.unknown += rhs.unknown;
  lhs.le10 += rhs.le10;
  lhs.le100 += rhs.le100;
  lhs.gt100 += rhs.gt100;
  lhs.unbounded += rhs.unbounded;

  return lhs;
}

static void qudo_metrics_header() {
  cout << "?" << g_program_options.delimiter
       << "<= 10" << g_program_options.delimiter
       << "<= 100" << g_program_options.delimiter
       << "100 <" << g_program_options.delimiter
       << "+oo";
}

static std::ostream& operator<<(std::ostream &os, const qudo_metrics_t &rhs) {
  char d = g_program_options.delimiter;
  os << rhs.unknown << d << rhs.le10 << d << rhs.le10 << d << rhs.le100
     << d << rhs.gt100 << d << rhs.unbounded;
  return os;
}

static bool is_null(const qudo_metrics_t& m) {
    return 0 == m.unknown
      && 0 == m.le10
      && 0 == m.le100
      && 0 == m.gt100
      && 0 == m.unbounded;
}
/* --------------------------------------------------------- metrics_t -- */

void metrics_initialize(metrics_t&m) {
  if (g_program_options.metrics_operator_count) {
    for (uint32_t i = 0; i < C_number_operators; ++i) {
      m.opco.counters.insert(pair<string, uint32_t>(v_operators_name[i], 0));
    }
  }
}

void metrics_header() {
  if (g_program_options.header) {
    bool first = true;
    if (g_program_options.metrics_hypotheses_count) {
      if (!first) {
	std::cout << g_program_options.delimiter;
      }
      hyco_metrics_header();
      first = false;
    }
    if (g_program_options.metrics_operator_count) {
      if (!first) {
	std::cout << g_program_options.delimiter;
      }
      opco_metrics_header();
      first = false;
    }
    if (g_program_options.metrics_quant_domain) {
      if (!first) {
	std::cout << g_program_options.delimiter;
      }
      qudo_metrics_header();
      first = false;
    }
    cout << endl;
  }
}

metrics_t& operator+=(metrics_t &lhs, const metrics_t& rhs) {
  if (g_program_options.metrics_hypotheses_count) {
    lhs.hyco += rhs.hyco;
  }
  if (g_program_options.metrics_operator_count) {
    lhs.opco += rhs.opco;
  }
  if (g_program_options.metrics_quant_domain) {
    lhs.qudo += rhs.qudo;
  }
  return lhs;
}

std::ostream &operator<<(std::ostream &os, const metrics_t &metrics) {
  char d = g_program_options.delimiter;
  bool first = true;
  if (g_program_options.metrics_hypotheses_count) {
    if (!first) {
      std::cout << d;
    }
    std::cout << metrics.hyco;
    first = false;
  }
  if (g_program_options.metrics_operator_count) {
    if (!first) {
      std::cout << d;
    }
    std::cout << metrics.opco;
    first = false;
  }
  if (g_program_options.metrics_quant_domain) {
    if (!first) {
      std::cout << d;
    }
    std::cout << metrics.qudo;
    first = false;
  }
  return os;
}

bool is_null(const metrics_t& m) {
  return (!g_program_options.metrics_hypotheses_count || is_null(m.hyco))
    && (!g_program_options.metrics_operator_count || is_null(m.opco))
    && (!g_program_options.metrics_quant_domain || is_null(m.qudo));
}

