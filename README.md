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
  fuint32_t varAIndex = solver.createVar(VariableType::INTEGER, 0.0, 10.0, 3.0); // coefficient of variable A in objective function is 3.0
  fuint32_t varBIndex = solver.createVar(VariableType::CONTINUOUS, 0.0, 5.0, 1.0);
  fuint32_t constraint = solver.createLinearConstraintLeq(5.5);
  solver.addToCst(constraint, varAIndex);      // coefficient of variable A is 1.0
  solver.addToCst(constraint, varBIndex, 2.0); // coefficient of variable B is 2.0
  solver.solve();
  if (solver.getSolution() != nullptr)
  {
    std::cout << "Value of A: " << solver.getVariableValue(varAIndex) << std::endl;
    std::cout << "Value of B: " << solver.getVariableValue(varBIndex) << std::endl;
  }
  return 0;
}
```
Output:
```
Value of A: 5.0
Value of B: 0.25
```

### Why do create methods return fuint32_t?
The return values are indices for fetching the variables from internal ```std::vector```.
You use the indices to get variables' values (after solving) using ```solver.getVariableValue(variableIndex)``` or ```solver.getBinaryValue(variableIndex)```.
Apart from that, you add variables to constraints using the indices using ```solver.addToCst(constraintIndex, variableIndex, 1.0)```.

## Examples
I wrote some examples for how to use the wrapper:
- [Independent set](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/src/independent_set.cpp): Find the maximum independent set in a graph using a MILP.
- [Vertex cover](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/src/vertex_cover.cpp): Find the minimum cost vertex cover in a graph using a MILP.
- [Dependency Knapsack](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/src/dependency_knapsack.cpp): Use as much capacity of a knapsack while taking into account dependencies of items (certain items can only be included in the knapsack if other items that the item depends on are included as well). 
- [n queens](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/src/n_queens.cpp): Find the maximum number of queens not interfering with each other that can be placed on a chess board.
- [min-cost flow](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/src/min_cost_flow.cpp): A classical example of a LP. Find the cheapest  flow in a graph with given flow value. Usually this is calculated using a network simplex algorithm (which is specialized and typically much faster). Regardless of that, one can see how to use this wrapper.
- [minimum team matching](https://github.com/keksklauer4/SCIP_Wrapper/blob/master/src/min_team_matching.cpp): This is a problem I found on Reddit. It is about forming teams with at least 3 and at most 5 members while minimizing costs. It turns out this problem is strongly NP-hard which I showed exemplarily through a reduction from [3-Partition](https://en.wikipedia.org/wiki/3-partition_problem). The reduction is implemented in the same file and two example problem instances of the 3-partition problem are solved using the reduction (both taken from Wikipedia).
