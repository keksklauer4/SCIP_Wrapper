#include "vertex_cover.hpp"

using namespace scip_wrapper;

void vertex_cover::solve_minimum_vertex_cover(std::vector<std::pair<fuint32_t, fuint32_t>> &edges,
                                std::vector<double> weights)
{
  SCIPSolver solver{"mvc_model", SolverSense::MINIMIZE};

  // reserve space for variables and constraints:
  solver.setNbVars(weights.size());
  solver.setNbCsts(edges.size());

  std::vector<fuint32_t> variables{};
  variables.reserve(weights.size());

  // create variables
  for (auto nodeWeightIt = weights.begin(); nodeWeightIt != weights.end(); ++nodeWeightIt)
  {
    variables.push_back(solver.createBinaryVar(*nodeWeightIt));
  }

  // create constraints
  for (auto pairIt = edges.begin(); pairIt != edges.end(); ++pairIt)
  {
    fuint32_t cstIndex = solver.createLinearConstraintGeq(1.0);
    solver.addToCst(cstIndex, variables.at(pairIt->first));
    solver.addToCst(cstIndex, variables.at(pairIt->second));
  }

  solver.solve();

  std::cout << (solver.getSolution() != nullptr ? "Sucess!" : "An error occurred...") << std::endl;

  std::cout << "Retrieving solution..." << std::endl;
  fuint32_t index = 0;
  for (auto nodeWeightIt = weights.begin(); nodeWeightIt != weights.end(); ++nodeWeightIt, index++)
  {
    double value = solver.getVariableValue(variables.at(index));
    std::cout << "Node " << index << " is " << value << " -> " << (value == 1.0) << std::endl;
  }
}
