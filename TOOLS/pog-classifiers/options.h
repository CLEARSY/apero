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
   Shares program options with all modules.

 */
#ifndef __OPTIONS_H
#define __OPTIONS_H

/// records the value for the command-line options
struct program_options_t {
  bool metrics_hypotheses_count;
  bool metrics_quant_domain;
  bool metrics_operator_count;
  bool header;
  bool verbose;
  char delimiter;
  int filter;
};
extern struct program_options_t g_program_options;

#endif /* __OPTIONS_H */
