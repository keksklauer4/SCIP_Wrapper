#include "qubo_examples.hpp"

using namespace scip_wrapper;
using namespace qubo_examples;

/* keksklauer4 27.03.2021
 *
 * Calculate a minimum weight vertex cover for a given graph using QUBO model.
 * For that we use the QUBOSolver
 */

void qubo_examples::solve_minimum_vertex_cover(std::vector<EdgePair> &edges,
                                std::vector<double> weights)
{
  QUBOSolver solver{SolverSense::MINIMIZE, 10000000.0};
  VariableVector vars{};
  vars.resize(weights.size());
  for (fuint32_t i = 0; i < weights.size(); i++)
  {
    fuint32_t varIndex = solver.createBinaryVar();
    vars.at(i) = varIndex;
    solver.addLinearTerm(varIndex, weights.at(i), false);
  }

  for (const auto& edge : edges)
  {
    // each edge must at least have one endpoint
    solver.addGeqTerm(vars.at(edge.first), vars.at(edge.second));
  }

  if (!solver.solve())
  {
    std::cerr << "Error occurred..." << std::endl;
  }

  double objective = 0.0;
  for (fuint32_t i = 0; i < weights.size(); i++)
  {
    if (solver.getBinaryValue(vars.at(i)))
    {
      std::cout << "Node " << i << " is included with cost " << weights[i] << std::endl;
      objective += weights[i];
    }
  }
  std::cout << "Total cost is " << objective << std::endl;
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

  qubo_examples::solve_minimum_vertex_cover(edges, weights);
}