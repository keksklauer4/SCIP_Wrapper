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