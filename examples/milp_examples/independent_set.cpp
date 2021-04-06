#include "milp_examples.hpp"

using namespace scip_wrapper;
using namespace milp_examples;

/* keksklauer4 16.03.2021
 *
 * Calculate a maximal independent set for a given graph using a MILP.
 */

void milp_examples::solve_independent_set(const std::vector<EdgePair> &edges,
                           const std::vector<scip_wrapper::fuint32_t> nodes)
{
  std::map<fuint32_t, fuint32_t> nodeIndices{}; // key: nodeID ; value: index of the corresponding variable

  MILPSolver solver{"IndependentSetTest", SolverSense::MAXIMIZE};

  // create variables
  for (auto it = nodes.begin(); it != nodes.end(); ++it)
  {
    fuint32_t varIndex = solver.createBinaryVar(1.0);
    nodeIndices.insert(std::make_pair(*it, varIndex));
  }

  // create constraints
  for (auto it = edges.begin(); it != edges.end(); ++it)
  {
    fuint32_t nodeIndexA = nodeIndices[it->first];
    fuint32_t nodeIndexB = nodeIndices[it->second];

    // for each edge at most one adjacent variable is allowed to be in the independent set
    fuint32_t cstIndex = solver.createLinearConstraintLeq(1.0);
    solver.addToCst(cstIndex, nodeIndexA);
    solver.addToCst(cstIndex, nodeIndexB);
  }

  solver.solve();

  // Check solution
  std::cout << (solver.getSolution() != nullptr ? "Sucess!" : "An error occurred...") << std::endl;

  if (solver.getSolution() == nullptr) return;

  // Display independent set:
  std::cout << "This is the independent set:" << std::endl;
  fuint32_t amountNodesIncluded = 0;
  for (auto it = nodeIndices.begin(); it != nodeIndices.end(); ++it)
  {
    fuint32_t variableIndex = it->second;
    if (solver.getBinaryValue(variableIndex))
    {
      std::cout << "Node " << it->first << " is in." << std::endl;
      amountNodesIncluded++;
    }
  }
  std::cout << amountNodesIncluded << " nodes in the independent set." << std::endl;
}

int main()
{
  std::vector<scip_wrapper::fuint32_t> nodes{
    0, 1, 2, 3, 4
  };

  std::vector<milp_examples::EdgePair> edges{
    std::make_pair(0,1),
    std::make_pair(0,3),
    std::make_pair(1,2),
    std::make_pair(3,4),
    std::make_pair(1,3),
    std::make_pair(2,4),
    std::make_pair(2,3)
  };

  milp_examples::solve_independent_set(edges, nodes);
}
