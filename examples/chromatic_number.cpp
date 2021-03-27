#include "milp_examples.hpp"

using namespace scip_wrapper;
using namespace milp_examples;

/* keksklauer4 18.03.2021
 *
 * Calculating the chromatic number of a graph.
 */

void milp_examples::chromatic_number(const std::vector<fuint32_t> &nodes,
                      const std::vector<EdgePair> &edges)
{
  MILPSolver solver{"chromaticNumberSolver", SolverSense::MINIMIZE};
  solver.setLogging();

  fuint32_t targetNumber = solver.createVarLowerBounded(CONTINUOUS, 0.0, 1.0);

  std::map<fuint32_t, fuint32_t> colorVars{};
  for (auto node : nodes)
  {
    fuint32_t colorVar = solver.createIntVar(1.0, nodes.size(), 0.0);
    colorVars.insert(std::make_pair(node, colorVar));
    fuint32_t objCst = solver.createLinearConstraintGeq(0.0);
    solver.addToCst(objCst, targetNumber);
    solver.addToCst(objCst, colorVar, -1.0);
  }

  for (const auto& edge : edges)
  {
    fuint32_t colorVarFrom = colorVars[edge.first];
    fuint32_t colorVarTo = colorVars[edge.second];
    solver.createUnequalConstraint(colorVarFrom, colorVarTo, 2*nodes.size());
    // 2*edges.size() is sufficiently large as the maximum chromatic number can be n
    // ( = all nodes have different color )
  }

  if (!solver.solve())
  {
    std::cerr << "Something failed..." << std::endl;
    return;
  }

  for (auto node : nodes)
  {
    std::cout << "Node " << node << " has color " << solver.getVariableValue(colorVars[node]) << std::endl;
  }
  for (const auto& edge : edges)
  {
    fuint32_t colorFrom = solver.getVariableValue(colorVars[edge.first]);
    fuint32_t colorTo   = solver.getVariableValue(colorVars[edge.second]);
    std::cout << "Edge " << edge.first << "---[" << colorFrom << "]---[" << colorTo << "]--->" << edge.second
              << (colorFrom == colorTo ? " Bug! Equal colors detected!" : "") << std::endl;
  }
  std::cout << "Chromatic number of the graph is " << solver.getVariableValue(targetNumber) << std::endl;
}


int main()
{
  // Example taken from Wikipedia: https://en.wikipedia.org/wiki/Graph_coloring
  std::vector<fuint32_t> nodes
  {
    0,1,2,3,4,5,6,7,8,9
  };
  std::vector<EdgePair> edges
  {
    EdgePair(0,1),
    EdgePair(1,2),
    EdgePair(2,3),
    EdgePair(3,4),
    EdgePair(4,0),

    EdgePair(0,5),
    EdgePair(1,6),
    EdgePair(2,7),
    EdgePair(3,8),
    EdgePair(4,9),

    EdgePair(5,7),
    EdgePair(7,9),
    EdgePair(9,6),
    EdgePair(6,8),
    EdgePair(8,5)
  };
  // std::vector<fuint32_t> nodes
  // {
  //   1,2,3,4,5
  // };
  // std::vector<EdgePair> edges
  // {
  //   EdgePair(1,2),
  //   EdgePair(2,3),
  //   EdgePair(3,4),
  //   EdgePair(4,5),
  //   EdgePair(5,1)
  // };
  milp_examples::chromatic_number(nodes, edges);
}