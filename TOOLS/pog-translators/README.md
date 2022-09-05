# TOOLS/pog-translators

Tooling to translate proof obligations from the POG format
to other formats/languages.

* BAST: a C++ library to represent B Abstract Syntax Trees. Depends on Qt5Core, Qt5Xml.

* POGLoader: a C++ library to load POG files to memory, using the BAST
  representation. Depends on BAST, Qt5Core, Qt5Xml.

* PPTRANSSMT: a C++ program to translate POG files to SMTLIB-2.6
  format, based on an extension of the pptrans encoding described in the paper [Integrationg SMT solvers in Rodin](http://dx.doi.org/10.3233/SAT190123).

## COMPILING

The code uses Qt libraries. It is known to compile with Qt5.12 under
Linux, but should be compatible with the latest 5.x LTS.

Our build process is based on `cmake`, which produces suitable Makefiles from the `CMakeLists.txt` provided here. To build the code, run the following commands
```
cmake .
make
```

## COPYING


This software is copyright (C) CLEARSY 2022. All rights reserved.

The source code is distributed under the terms of the GNU General Public Licence (GNU GPL) Version 3.

