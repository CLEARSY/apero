/*
   pogmetrics (Produces miscellaneous metrics on POG files).
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

/**   Prints out different metrics about B proof obligations stored in a .pog file.

      The user has to set which metrics are printed via command-line options.

   Quick user notice:

   The program has a filter option that may be used to change the set of 
   hypothesis that are counted in the metrics. The program handles 5 different
   values for this option.
   - 0: goal only
   - 1: goal and the local hypotheses (Hypothesis and Ref_Hyp in pog file)
   - 2: goal and all hypotheses (Hypothesis, Ref_Hyp and Definition in pog file)
   - 3: goal and all hypotheses (as in 2) having a common symbol with the goal
   - 4: goal and the local hypotheses and all global hypotheses (Definition in
   pog file) having a common symbol with the goal.

   Option --metrics-hypotheses-count enables metrics on the number of hypotheses. 
   Two metrics are printed: the number of global hypotheses, and the number of local hypotheses.
   When the filter is 0, both numbers are 0.
   When the filter is 1, the first number is 0.

   Option --metrics-quantification-domain enables metrics on the size of the
   domains of quantified variables.

   Option --metrics-operator-count enables metrics on the number of occurrences
   of B operators.

   Caveat:
   - metrics are represented by 32-bit unsigned integers, overflows are
   not detected
*/
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using std::array;
using std::map;
using std::move;
using std::pair;
using std::string;
using std::vector;

using std::cerr;
using std::clog;
using std::cout;
using std::endl;

static void help() {
  cerr << "pogmetrics: computes miscellaneous metrics on a POG file" << endl
       << "(c) 2022, CLEARSY" << endl
       << "This program comes with ABSOLUTELY NO WARRANTY." << endl
       << "This is free software, and you are welcome to redistribute it" << endl
       << "under certain conditions." << endl
       << endl
       << "Usage:\tpogmetrics [options] file\n" << endl
       << "\twhere options might be" << endl
       << "\t\t--metrics-hypotheses-count: counts global and local hypotheses" << endl
       << "\t\t--metrics-quantification-domain: estimates domain cardinality for quantified variables" << endl
       << "\t\t--metrics-operator-count: counts occurrences of B operators" << endl
       << "\t\t-H, --header:    prints table header line;" << endl
       << "\t\t-d c, --delimiter=c:   Uses c as a character delimiter;" << endl
       << "\t\t-f n, --filter=n (0 <= f <= 4):   Chooses hypotheses filter (default = 2);" << endl
       << "\t\t-v , --verbose:   prints additional messages (may be useful for "
    "developers);"
       << endl;
}

#include "pugipog.h"
#include "pugixml.hpp"

using namespace pugi;

// module that computes metrics based on domains of quantified variables
#include "metrics.h"

// module that computes sets of free symbols in B terms, combines and compares them
#include "symbols.h"

/// different modes to compute metrics
enum coverage_t {
		 G0 = 0, /// goal
		 G1 = 1, /// goal and local hypotheses
		 G2 = 2, /// goal and all hypotheses
		 G3 = 3, /// G0 and all hypotheses with a symbol common with G0
		 G4 = 4  /// G1 + global hypotheses with a symbol common with G1
};

#include "options.h"

/// records the value for the command-line options
struct program_options_t g_program_options = {
					      .metrics_hypotheses_count = false,
					      .metrics_quant_domain = false,
					      .metrics_operator_count = false,
					      .header = false,
					      .verbose = false,
					      .delimiter = ',',
					      .filter = G2
};

typedef map<string, metrics_t *> metrics_table_t;

/** computes and prints metrics */
static void metrics(const xml_node &root, const int coverage);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    help();
    return EXIT_FAILURE;
  }

  while (true) {
    enum {
	  M_HYPOTHESES_COUNT = 256,
	  M_QUANTIFICATION_DOMAIN = 257,
	  M_OPERATOR_COUNT = 258
    };
    
    const char *const short_options = "?Hd:f:v";
    static struct option long_options[] = {
					   {"metrics-hypotheses-count", no_argument, nullptr, M_HYPOTHESES_COUNT},
					   {"metrics-quantification-domain", no_argument, nullptr, M_QUANTIFICATION_DOMAIN},
					   {"metrics-operator-count", no_argument, nullptr, M_OPERATOR_COUNT},
					   {"header", no_argument, nullptr, 'H'},
					   {"delimiter", required_argument, nullptr, 'd'},
					   {"filter", required_argument, nullptr, 'f'},
					   {"verbose", no_argument, nullptr, 'v'},
					   {nullptr, no_argument, nullptr, 0}
    };
    const int opt =
      getopt_long(argc, argv, short_options, long_options, nullptr);
    if (-1 == opt)
      break;
    switch (opt) {
    case 'H':
      g_program_options.header = true;
      break;
    case 'd':
      g_program_options.delimiter = optarg[0];
      break;
    case 'f':
      g_program_options.filter = atoi(optarg);
      break;
    case 'v':
      g_program_options.verbose = true;
      break;
    case M_HYPOTHESES_COUNT:
      g_program_options.metrics_hypotheses_count = true;
      break;
    case M_QUANTIFICATION_DOMAIN:
      g_program_options.metrics_quant_domain = true;
      break;
    case M_OPERATOR_COUNT:
      g_program_options.metrics_operator_count = true;
      break;
    case 'h':
    case '?':
    default:
      help();
      return EXIT_SUCCESS;
    }
  }
  if (argc < optind + 1) {
    help();
    return EXIT_FAILURE;
  }
  if (!(G0 <= g_program_options.filter && g_program_options.filter <= G4)) {
    cerr << "Error: filter should be between in 0..4." << endl;
    return EXIT_FAILURE;
  }
  if (!g_program_options.metrics_hypotheses_count &&
      !g_program_options.metrics_quant_domain &&
      !g_program_options.metrics_operator_count) {
    cerr << "Error: no metrics selected." << endl;
  }

  const char *pog_path = argv[optind];
  xml_document pog_doc;
  xml_parse_result pog_parse = pog_doc.load_file(pog_path);

  if (pog_parse.status != status_ok) {
    cerr << "Cannot load file " << pog_path << " into DOM object" << endl;
    exit(EXIT_FAILURE);
  }

  metrics(pog_doc.child("Proof_Obligations"), g_program_options.filter);
}

/** metrics update conditioned on existence of common symbols */
static void update_metrics_if(metrics_t& dst, const metrics_t& src, const symbol_set_t& s1, const symbol_set_t& s2) {
  if (symbol_set_intersects(s1, s2)) {
    dst += src;
  }
}

/*
  Structure of a proof obligation file
  Proof_Obligations
  +- Define* name="xxx"
  |  +- FORMULA*
  +- Proof_Obligation
  |  +- Definition* name="xxx"
  |  +- Hypothesis*
  |  |  +- FORMULA
  |  +- Local_Hyp* num="nnn"
  |  |  +- FORMULA
  |  +- Simple_Goal*
  |  |  +- Ref_Hyp* num="nnn"
  |  |  +- Goal
  |  |  |  +- FORMULA
*/

/*
  The following routine computes metrics
  on a POG file. This routine is such that:
  - the computation implements the different hypotheses filters 0-4.
  - when a formula metrics is null, the filter is not considered.
  (note that one could conversely use the filtering condition to
  skip the computation of the metrics of some hypotheses).
*/

static void metrics(const xml_node &root, const int coverage) {
  
  bounds_table_t table; // used with metrics on domains of quantified variables
  
  if (g_program_options.metrics_quant_domain) {
    /* preprocess file to compute symbol table */
    eval_bounds(root, table);
  }
  
  /* For each hypothesis in each 'Define' section, compute the associated
     metrics.

     A string-indexed map contains the values by 'Define' section.

     When coverage is G3 or G4, some hypotheses may be discarded, so the metrics
     need to be computed individually for each hypothesis.
     The values are stored as follows:
     - The values for each 'Define' section are stored in a vector.
     - The i-th hypothesis in the section is stored at position i-1 of the vector.

     When coverage is G2, all hypotheses are considered. The values
     are stored in a single domain_metrics structure.

     Otherwise, the global hypotheses are not considered and no value
     needs to be computed.

     For modes G4 and G4, it is also necessary to know the symbols in
     the hypotheses. During the traversal over the 'Define' sections,
     the set of symbols appearing in each hypothesis is computed, but
     only for those hypotheses where the metrics count is not
     null. Indeed, when the metrics count is null, then it is useless
     to take these metrics into account. So, in the case the metrics
     count is null in an hypothesis, the empty set of symbols will
     be associated to this hypothesis (instead of the set of symbols
    appearing in the hypothesis).
		      */
  map<string,vector<metrics_t *>> define_metrics_g34;
  map<string,metrics_t *> define_metrics_g2;
  
  map<string,vector<symbol_set_t>> define_symbols_g34;

  metrics_header();
  bool trace = false;
  if(trace) clog << "Processing Define elements" << endl;
  if (G3 <= coverage) {
    if (trace) clog << "G3 <= coverage" << endl;
    for (auto d : root.children("Define")) {
      string tag(d.attribute("name").value());
      vector<metrics_t *> metrics;
      vector<symbol_set_t> v;
      if(trace) clog << "Define " << tag << endl;
      for (auto f : d.children()) {
	metrics_t *m = new metrics_t;
	metrics_initialize(*m);
	if (trace) clog << "formula" << endl;
	if (g_program_options.metrics_hypotheses_count) {
	  m->hyco.global += 1;
	}
	if (g_program_options.metrics_operator_count) {
	  opco_metrics_formula(m->opco, f);
	}
	if (g_program_options.metrics_quant_domain) {
	  qudo_metrics_formula(table, f, m->qudo);
	}
	metrics.push_back(m);
	symbol_set_t s;
	if (trace) clog << "metrics " << *m << endl;
	if (!is_null(*m)) {
	  get_all_symbols(s, f);
	  if (trace) clog << "symbols " << s << endl;
	}
	v.push_back(move(s));
      }
      define_metrics_g34.insert(pair<string,vector<metrics_t*>>(tag, move(metrics)));
      define_symbols_g34.insert(pair<string,vector<symbol_set_t>>(tag, move(v)));
    }
  } else if (G2 == coverage) {
    if (trace) clog << "G2 == coverage" << endl;
    for (auto d : root.children("Define")) {
      metrics_t *m = new metrics_t;
      string tag = d.attribute("name").value();
      metrics_initialize(*m);
      if(trace) clog << "Define " << tag << endl;
      for (auto f : d.children()) {
	if (g_program_options.metrics_hypotheses_count) {
	  m->hyco.global += 1;
	}
	if (g_program_options.metrics_operator_count) {
	  opco_metrics_formula(m->opco, f);
	}
	if (g_program_options.metrics_quant_domain) {
	  qudo_metrics_formula(table, f, m->qudo);
	}
      }
      if (trace) clog << "metrics " << *m << endl;
      define_metrics_g2.insert(pair<string, metrics_t *>(tag, m));
    }
  }
  
  if(trace) clog << "Processing Proof_Obligation elements" << endl;
  for (const auto po : root.children("Proof_Obligation")) {
    /*
      For each 'Hypothesis', compute the associated metrics.
      
      In G3 and G4 mode,
      the value for each 'Hypothesis' is stored in a vector,
      the value for the i-th 'Hypothesis' being stored at position i-1.

      In G1 and G2 mode,
      the values for all 'Hypothesis' are cumulated in a single record.

      In G2 mode,
      the values for all referenced 'Define' are cumulated in a single record.

      For each 'Local_Hyp', compute the associated metrics.

      In modes G1-G4,
      the values for each 'Local_Hyp' is stored in a map,
      indexed by the 'num' attribute of the 'Local_Hyp'.

      In mode G3, we need to take into account the metrics of those local 
      hypotheses that have a common symbol with the goal. A set of symbols
      is associated to each local hypothesis:
      - an empty set of symbols when the metrics are null
      - the set of symbols in the hypothesis otherwise.

      In mode G4, we need to take into account the metrics of the global
      hypotheses that have a common symbol with the goal and the local
      hypotheses. A set of symbols is associated with each local hypothesis.
    */
    vector<metrics_t *> hypothesis_metrics_g34;
    vector<symbol_set_t> hypothesis_symbols_g34;
    
    metrics_t hypothesis_metrics_g12; // cumulated metrics for all 'Hypothesis' elements
    map<string,metrics_t *> local_hyp_metrics;
    map<string,symbol_set_t> local_hyp_symbols_g34;

    metrics_t definition_metrics_g2; // cumulated metrics for all 'Define' referenced by a 'Definition'
    
    metrics_initialize(hypothesis_metrics_g12);
    metrics_initialize(definition_metrics_g2);
    if(trace) clog << "Proof_Obligation" << endl;
    if (G1 == coverage || G2 == coverage) {
      for (const auto h : po.children("Hypothesis")) {
	if(g_program_options.metrics_hypotheses_count) {
	  hypothesis_metrics_g12.hyco.local += 1;
	}
	if (g_program_options.metrics_operator_count) {
	  opco_metrics_formula(hypothesis_metrics_g12.opco, h);
	}
	if (g_program_options.metrics_quant_domain) {
	  qudo_metrics_formula(table, h, hypothesis_metrics_g12.qudo);
	}
      }
      if (trace) clog << "hypothesis_metrics_g12: " << hypothesis_metrics_g12 << endl;
    }
    if (G2 == coverage) { 
      for (const auto d: po.children("Definition")) {
        string tag = d.attribute("name").value();
        const auto& p = define_metrics_g2.find(tag);
        if (p == define_metrics_g2.end()) {
          if (g_program_options.verbose) {
            cerr << "Error: Definition name does not match a Define in pog file." << endl;
          }
        }
        else {
	  definition_metrics_g2 += *p->second;
        }
	if (trace) clog << "definition_metrics_g2: " << definition_metrics_g2 << endl;
      }
    } else if (G3 == coverage || G4 == coverage) {
      for (const auto h : po.children("Hypothesis")) {
        metrics_t *m = new metrics_t;
	symbol_set_t s;
	metrics_initialize(*m);
	if (trace) clog << "Hypothesis" << endl;
	if (g_program_options.metrics_hypotheses_count) {
	  m->hyco.local += 1;
	}
	if (g_program_options.metrics_operator_count) {
	  opco_metrics_formula(m->opco, h.first_child());
	}
	if (g_program_options.metrics_quant_domain) {
	  qudo_metrics_formula(table, h.first_child(), m->qudo);
	}
	if (trace) clog << "metrics: " << *m << endl;
        hypothesis_metrics_g34.push_back(m);
	if (G4 == coverage || !is_null(*m)) {
  	  get_all_symbols(s, h.first_child());
	  if (trace) clog << "symbols " << s << endl;
	}
	hypothesis_symbols_g34.push_back(move(s));
      }
    }
    if (G1 <= coverage) {
      for (const auto lh : po.children("Local_Hyp")) {
        string num = lh.attribute("num").value();
        metrics_t *m = new metrics_t;
	metrics_initialize(*m);
	if (trace) clog << "Local_Hyp" << endl;
	if (g_program_options.metrics_hypotheses_count) {
	  m->hyco.local += 1;
	}
	if (g_program_options.metrics_operator_count) {
	  opco_metrics_formula(m->opco, lh.first_child());
	}
	if (g_program_options.metrics_quant_domain) {
	  qudo_metrics_formula(table, lh.first_child(), m->qudo);
	}
	if (trace) clog << "metrics: " << *m << endl;
        local_hyp_metrics.insert(pair<string, metrics_t *>(num, move(m)));
	if (G3 == coverage || G4 == coverage) {
	  symbol_set_t s;
	  if (G4 == coverage || !is_null(*m)) {
	    get_all_symbols(s, lh.first_child());
	    if (trace) clog << "symbols " << s << endl;
	  }
	  local_hyp_symbols_g34.insert(pair<string, symbol_set_t>(num, move(s)));
	}
      }
    }

    if(trace) clog << "Processing Simple_Goal elements" << endl;
    for (const auto simple_goal: po.children("Simple_Goal")) {

      const auto goal = simple_goal.child("Goal").first_child();
      if (trace) clog << "Simple_Goal" << endl;

      metrics_t goal_metrics;
      metrics_initialize(goal_metrics);
      if (g_program_options.metrics_operator_count) {
	opco_metrics_formula(goal_metrics.opco, goal);
      }
      if (g_program_options.metrics_quant_domain) {
	qudo_metrics_formula(table, goal, goal_metrics.qudo);
      }
      
      symbol_set_t goal_symbols;
      if (coverage == G3 || coverage == G4) {
	get_all_symbols(goal_symbols, goal);
      }
  
      /*
	For modes G1 and G2,
	add unconditionnally all 'Hypothesis'
	add all 'Local_Hyp' that are referenced.
      */
      if (G1 == coverage || G2 == coverage) {
	goal_metrics += hypothesis_metrics_g12;
 	for(const auto rh: simple_goal.children("Ref_Hyp")) {
	  string num (rh.attribute("num").value());
	  auto itr = local_hyp_metrics.find(num);
	  if (itr == local_hyp_metrics.end()) {
	    if (g_program_options.verbose) {
	      cerr << "Error: local hypothesis reference does not resolve to an existing hypothesis." << endl;
	    }
	    continue;
	  }
	  goal_metrics += *itr->second;
	}
      }
      /* 
	 For mode G2 only,
	 add unconditionnally all metrics of the referenced `Defines`
      */
      if (G2 == coverage) {
	goal_metrics += definition_metrics_g2;
      }
      /* 
	 For mode G3 only,
	 add all metrics for the global hypotheses ('Definition')
	 add all metrics for the local hypotheses ('Hypothesis' and 'Ref_Hyp')
	 when they have a symbol common with the goal
      */
      if (G3 == coverage) {
	for(const auto definition: simple_goal.children("Definition")) {
	  string tag = definition.attribute("name").value();
	  const auto& ps = define_symbols_g34.find(tag);
	  const auto& pm = define_metrics_g34.find(tag);
	  if (ps == define_symbols_g34.end()) {
	    if (g_program_options.verbose) {
	      cerr << "Error: Definition name does not match a Define in pog file [define_symbols_g34]." << endl;
            }
	    continue;
          }
	  if (pm == define_metrics_g34.end()) {
	    if (g_program_options.verbose) {
	      cerr << "Error: Definition name does not match a Define in pog file [define_metrics_g34]." << endl;
            }
	    continue;
	  }
	  vector<symbol_set_t>& vs = ps->second;
	  vector<metrics_t*>& vm = pm->second;
	  for(auto i = 0u; i < vs.size(); ++i) {
	    update_metrics_if(goal_metrics, *vm.at(i), goal_symbols, vs.at(i));
	  }
        }
	for (auto i = 0u; i < hypothesis_symbols_g34.size(); ++i) {
	  update_metrics_if(goal_metrics, *hypothesis_metrics_g34.at(i),
			    goal_symbols, hypothesis_symbols_g34.at(i));
        }
	for(const auto rh: simple_goal.children("Ref_Hyp")) {
	  string num (rh.attribute("num").value());
	  auto pm = local_hyp_metrics.find(num);
	  if (pm == local_hyp_metrics.end()) {
	    if (g_program_options.verbose) {
	      cerr << "Error: Ref_Hyp num does not match a Local_Hyp in pog file [local_hyp_metrics]." << endl;
	    }
	    continue;
	  }
	  auto ps = local_hyp_symbols_g34.find(num);
	  if (ps == local_hyp_symbols_g34.end()) {
	    if (g_program_options.verbose) {
	      cerr << "Error: Ref_Hyp num does not match a Local_Hyp in pog file [local_hyp_symbols_g34]." << endl;
	    }
	    continue;
	  }
	  update_metrics_if(goal_metrics, *pm->second, goal_symbols, ps->second);
	}
      }
      /*
	G4 : Let basis be (goal and local hypotheses)
	G4 counts metrics for basis + global hypotheses with symbols in common with basis 

	Firstly, compute the metrics for the basis and the set of symbols in the basis.
	Then, for each 'Define' referred to by a 'Definition', 
	for each hypothesis in this 'Define'
	if this hypothesis has symbols common with basis,
	add the metrics associated to this hypothesis.
      */
      if (G4 == coverage) {
	symbol_set_t basis_symbols = goal_symbols; // we do want a copy of goal_symbols
	for (const auto& hs: hypothesis_symbols_g34) {
	  basis_symbols += hs;
	}
	for (const auto rh: simple_goal.children("Ref_Hyp")) {
	  const string num {rh.attribute("num").value()};
	  const auto& lhs {local_hyp_symbols_g34.find(num)};
	  if (lhs == local_hyp_symbols_g34.end()) {
	    if (g_program_options.verbose) {
	      cerr << "Error: Ref_Hyp num does not match a Local_Hyp in pog file [local_hyp_symbols_g34]." << endl;
            }
	    continue;
	    basis_symbols += lhs->second;
	  }
	}
	if (trace) clog << "basis_symbols: " << basis_symbols << endl;
	for(const auto definition: simple_goal.children("Definition")) {
	  string tag = definition.attribute("name").value();
	  const auto& ps = define_symbols_g34.find(tag);
	  const auto& pm = define_metrics_g34.find(tag);
	  if (ps == define_symbols_g34.end()) {
	    if (g_program_options.verbose) {
	      cerr << "Error: Definition name does not match a Define in pog file [define_symbols_g34]." << endl;
            }
	    continue;
          }
	  if (pm == define_metrics_g34.end()) {
	    if (g_program_options.verbose) {
	      cerr << "Error: Definition name does not match a Define in pog file [define_metrics_g34]." << endl;
            }
	    continue;
	  }
	  const vector<symbol_set_t>& define_symbols = ps->second;
	  const vector<metrics_t*>& define_metrics = pm->second;
	  for (auto i = 0u; i < define_symbols.size(); ++i) {
	    update_metrics_if(goal_metrics, *define_metrics.at(i), basis_symbols, define_symbols.at(i));
	  }
	}
      }
      cout << goal_metrics << endl;
    }

    /* free allocated memory */
    if (G3 == coverage || G4 == coverage) {
      for (auto p : hypothesis_metrics_g34) {
        delete p;
      }
    }
    if (G1 <= coverage) {
      for (auto p : local_hyp_metrics) {
        delete p.second;
      }
    }
  }
  /* free allocated memory */
  if (G3 <= coverage) {
    for (auto d : define_metrics_g34) {
      for (auto p : d.second) {
        delete p;
      }
    }
  } else if (G2 == coverage) {
    for (auto d : define_metrics_g2) {
      delete d.second;
    }
  }
}

