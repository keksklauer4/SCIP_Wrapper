# SCIP_Wrapper
A quick and dirty wrapper for SCIP (MILP solver) to abstract away the dirty details...

Used SCIP the first time and thought it's pretty dirty business (specifically, if one does not need all that extra stuff).
Thus, I quickly wrote a wrapper class for that and some simple example solving MILPs. Obviously, this can be used for linear programming as well.

Obviously, this is abstracting away quite a lot.

You can use the code, it's all yours. I do not guarantee anything except for the fact that whatever you do with this, it is none of my business.
Apart from that do not forget to cite SCIP.

## Overview

### Creating a minimal model
```C++
#include "scip_wrapper.hpp"

using namespace scip_wrapper;

/*
 * The modell:
 * Variables x_a and x_b
 * Objective: max { 3.0 * x_a + x_b }
 *
 * Constraints:
 * x_a + 2.0 * x_b <= 5.5
 * 0.0 <= x_a <= 10.0
 * 0.0 <= x_b <= 5.0
 * x_a as integer variable
 * x_b as real valued variable
 */

int main()
{
  SCIPSolver solver{"modelName", SolverSense::MAXIMIZE};
  fuint32_t varAIndex = solver.createIntVar(0.0, 10.0, 3.0); // coefficient of variable A in objective function is 3.0
  fuint32_t varBIndex = solver.createVar(VariableType::CONTINUOUS, 0.0, 5.0, 1.0);
  fuint32_t constraint = solver.createLinearConstraintLeq(5.5);
  solver.addToCst(constraint, varAIndex);      // coefficient of variable A is 1.0
  solver.addToCst(constraint, varBIndex, 2.0); // coefficient of variable B is 2.0
  if (solver.solve())
  {
    std::cout << "Value of A: " << solver.getVariableValue(varAIndex) << std::endl;
    std::cout << "Value of B: " << solver.getVariableValue(varBIndex) << std::endl;
  }
  return 0;
}
```
Output:
```
Value of A: 5
Value of B: 0.25
```

### Why do create methods return fuint32_t?
The return values are indices for fetching the variables from internal ```std::vector```.
You use the indices to get variables' values (after solving) using ```solver.getVariableValue(variableIndex)``` or ```solver.getBinaryValue(variableIndex)```.
Apart from that, you add variables to constraints using the indices using ```solver.addToCst(constraintIndex, variableIndex, 1.0)```.

## How to run?
You will have to get [SCIP](https://www.scipopt.org/index.php#download), more specifically, the scipoptsuite (I tested with version ```7.0.1```).
I built it using ```make install```. 
Then just run ```make all``` in the root folder of this project and the binaries for the different examples should lie in ```bin/```.
You can build specific examples using their respective name (for examples ```make rectangle_packing``` since the file is called ```rectangle_packing.cpp```).
Thus,

```make all``` for all examples.

```make <name>``` for specific examples (```<name>``` could be ```chromatic number```, ```tsp``` etc.)

## Examples
I wrote some examples for how to use the wrapper:
- [Independent set](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/example/independent_set.cpp): Find the maximum independent set in a graph using a MILP.
- [Vertex cover](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/example/vertex_cover.cpp): Find the minimum cost vertex cover in a graph using a MILP.
- [Dependency Knapsack](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/example/dependency_knapsack.cpp): Use as much capacity of a knapsack while taking into account dependencies of items (certain items can only be included in the knapsack if other items that the item depends on are included as well).
- [n queens completion](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/example/n_queens_completion.cpp): Find the maximum number of queens not interfering with each other that can be placed on a chess board with some queens already placed. Without queens preplaced on the board, thus, for the n queens problem, there are polynomial time algorithms solving the problem. With queens preplaced on the board, the problem is NP-hard.
- [min-cost flow](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/example/min_cost_flow.cpp): A classical example of a LP. Find the cheapest  flow in a graph with given flow value. Usually this is calculated using a network simplex algorithm (which is specialized and typically much faster). Regardless of that, one can see how to use this wrapper.
- [TSP](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/examples/tsp.cpp): The general travelling salesman problem as an ILP as formulated by Miller,Tucker and Zemlin. For an explanation, see the [Wikipedia article](https://en.wikipedia.org/wiki/Travelling_salesman_problem#Integer_linear_programming_formulations).
- [minimum team matching](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/examples/min_team_matching.cpp): This is a problem I found on Reddit. It is about forming teams with at least 3 and at most 5 members while minimizing costs. It turns out this problem is strongly NP-hard which I showed exemplarily through a reduction from [3-Partition](https://en.wikipedia.org/wiki/3-partition_problem). The reduction is implemented in the same file and two example problem instances of the 3-partition problem are solved using the reduction (both taken from Wikipedia).
- [Longest Path bounded degree 3](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/examples/longest_path_catan.cpp): It's about finding the longest edge disjoint path in a cyclic graph with maximum degree of 3. A vertex can be travelled to multiple times (trivially, at most twice). Start and end vertices are not predefined. The problem is motivated by a board game, [Siedler von Catan](https://www.catan.de/), in which one gets points for the longest trade route. Btw. I am not sure whether this problem is NP-hard (probably it is).
- [Max Clique](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/examples/max_clique.cpp): Find a maximum cardinality subset of vertices that are mutually adjacent. The problem is NP-hard and the decision version is among Karp's 21 NP-complete problems. The reduction from 3SAT to max clique is quite elegant.
- [Chromatic number](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/examples/chromatic_number.cpp): Find the minimum number of distinct colors needed to color each vertex with adjacent vertices being colored using distinct colors. The problem is NP-hard as well and is just as max clique among Karp's 21 problems proven to be NP-complete.
- [3-Partition](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/examples/three_partition.cpp): Given a multiset of ```n = 3*m``` integers, summing to ```t*m```, find ```m``` triplets ```S_1, ..., S_m``` with mutually distinct elements that each sum to ```t```. Trivially, every element in the multiset is in exactly one triplet. The problem is strongly NP-hard and is pretty useful with regard to reductions to 3-dimensional matching and tripartite matching.
- [Maximum weighted cut](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/examples/weighted_max_cut.cpp): Partition a given graph with capacities on edges such that the sum of capacities of edges cut by the partition (edges between vertices in distinct sets of the partitioning) is maximized. The problem is NP-hard.
- [2D rectangle packing](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/examples/rectangle_packing.cpp): Given a parent rectangle and a list of child rectangles, try to make use of as much area of the parent are by fitting non-overlapping child rectangles into the parent rectangle. Each child rectangle can only be used once and cannot be cut or changed (in this case not rotated either). The problem is NP-hard.
