#include "milp_examples.hpp"

using namespace scip_wrapper;
using namespace milp_examples;

/* keksklauer4 16.03.2021
 *
 * Calculate a minimum weight vertex cover for a given graph using a MILP.
 */

void milp_examples::solve_minimum_vertex_cover(std::vector<EdgePair> &edges,
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

  if (solver.getSolution() == nullptr) return;

  std::cout << "Retrieving solution..." << std::endl;
  fuint32_t index = 0;
  double totalCost = 0.0;
  for (auto nodeWeightIt = weights.begin(); nodeWeightIt != weights.end(); ++nodeWeightIt, index++)
  {
    if (solver.getBinaryValue(variables.at(index)))
    {
      std::cout << "Node " << index << " is in (" << *nodeWeightIt << ")." << std::endl;
      totalCost += *nodeWeightIt;
    }
  }

  std::cout << std::endl
            << "Total cost: " << totalCost << std::endl;
}

#define PAIR(a,b) std::make_pair(a,b)

int main()
{
  std::vector<double> weights
    { 1, 3, 3, 2, 2};
  std::vector<std::pair<fuint32_t, fuint32_t>> edges
  {
      PAIR(0,1),
      PAIR(0,3),
      PAIR(1,3),
      PAIR(1,2),
      PAIR(3,4),
      PAIR(2,3),
      PAIR(2,4)
  };

  milp_examples::solve_minimum_vertex_cover(edges, weights);

}