# pog-classifiers

Automate generation of classification data for POG files.

  * **pogmetrics** provides metrics on POG files.

## pogmetrics

Produces output in CSV format. One line is produced for each goal appearing in the POG file.

This program has the following command-line options:
  * -H : outputs a one-line header
  * -d c : uses character c as a field delimiter (default is comma)
  * -f n (0 <= n <= 4) : uses n as an hypothesis filter 
    * 0 collects the metrics in the goal
  	* 1 collects the metrics in the goal and the local hypotheses
	* 2 collects the metrics in the goal, and the local and global hypotheses **default**
    * 3 collects the metrics in the goal and the hypotheses having a common symbol with the goal
    * 4 collects the metrics in the goal, the local hypotheses, and the global 
	  hypotheses having a common symbol with the goal
    In cases 1 to 4, only the hypotheses associated to that goal are considered.
  * --metrics-hypotheses-count: counts global and local hypotheses
  * --metrics-quantification-domain: estimates domain cardinality for quantified variables
  * --metrics-operator-count: counts occurrences of B operators

pogmetrics is written in C++ and uses library [pugixml](https://pugixml.org/).
