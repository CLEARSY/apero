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
   Metrics data structure.

   Quick user notice:

   See data type `metrics_t` for details on the metrics computed.

 */
#ifndef __METRICS_H
#define __METRICS_H

#include<iostream>
#include<string>

#include "bounds.h"

/// hypotheses counting metrics
typedef struct hyco_metrics_t {
  uint32_t global = 0u; /// number of global hypotheses
  uint32_t local = 0u;  /// number of local hypotheses
} hyco_metrics_t;

/// operator occurrences counting metrics
typedef struct opco_metrics_t {
  std::map<std::string, uint64_t> counters;
  bool null = true; /// shortcut to remember if a counter is non zero
} opco_metrics_t;

/// quantified variables domain sizes metrics
typedef struct qudo_metrics_t {
  uint32_t unknown = 0; /// no information could be gathered for the domain size
  uint32_t le10 = 0;    /// domain size is at most 10
  uint32_t le100 = 0;   /// domain size is at most 100
  uint32_t gt100 = 0;   /// domain size is bounded and may be greater than 100
  uint32_t unbounded = 0; /// no bound known on the domain size

  /// when the metrics is considered null
  bool null() const {
    return 0 == unknown && 0 == le10 && 0 == le100 && 0 == gt100 && 0 == unbounded;
  }
} qudo_metrics_t;

typedef struct metrics_t {
  hyco_metrics_t hyco;
  opco_metrics_t opco;
  qudo_metrics_t qudo;
} metrics_t;

extern void metrics_header();
extern void metrics_initialize(metrics_t&m);
extern metrics_t& operator+=(metrics_t &lhs, const metrics_t& rhs);
extern std::ostream &operator<<(std::ostream &os, const metrics_t &rhs);
  
extern bool is_null(const metrics_t& m);

extern void opco_metrics_formula(opco_metrics_t &metrics, xml_node n);
extern void qudo_metrics_formula(const bounds_table_t& table, const xml_node n, qudo_metrics_t &metrics);
#endif /* __METRICS_H */
