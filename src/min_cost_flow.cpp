#include "milp_examples.hpp"

using namespace scip_wrapper;
using namespace milp_examples;

/* keksklauer4 16.03.2021
 *
 * Solving the min cost flow problem by formulating a LP and solving it.
 */

void milp_examples::solve_mincost_maxflow(const NodeIDVec &nodes, const MaxFlowEdgeVec &edges,
        fuint32_t sNode, fuint32_t tNode, double targetFlow)
{
  std::map<fuint32_t, fuint32_t> variableIndices{};
  std::vector<fuint32_t> edgeVars{};
  edgeVars.reserve(edges.size());

  SCIPSolver solver{"MinCostFlowSolver", SolverSense::MINIMIZE};

  for (auto node : nodes)
  {
    // create a constraint for every node to allow for flow
    double rhs = 0.0;
    if (node == sNode) rhs = -targetFlow;
    else if (node == tNode) rhs = targetFlow;
    fuint32_t cst = solver.createLinearConstraintEq(rhs);
    variableIndices.insert(std::make_pair(node, cst));
  }

  for (const auto &edge : edges)
  {
    fuint32_t edgeVarIndex = solver.createVar(VariableType::CONTINUOUS, 0.0, edge.capacity, edge.cost);
    fuint32_t fromNodeCst = variableIndices[edge.fromNode];
    fuint32_t toNodeCst = variableIndices[edge.toNode];
    solver.addToCst(fromNodeCst, edgeVarIndex, -1.0);
    solver.addToCst(toNodeCst, edgeVarIndex, 1.0);
    edgeVars.push_back(edgeVarIndex);
  }

  solver.solve();

  // Check solution
  std::cout << (solver.getSolution() != nullptr ? "Sucess!" : "An error occurred...") << std::endl;

  std::cout << "The solution:" << std::endl;
  double cost = 0.0;
  for (fuint32_t index = 0; index < edges.size(); index++)
  {
    double flowValue = solver.getVariableValue(edgeVars.at(index));
    auto &edge = edges.at(index);
    std::cout << "Flow on edge (" << edge.fromNode << ")---["
      << flowValue << "]--->(" << edge.toNode << ");" << std::endl;
    cost += flowValue * edge.cost;
  }
  std::cout << "Total cost is " << cost << "." << std::endl;
}

int main()
{
  NodeIDVec nodes{0,1,2,3};
  MaxFlowEdgeVec edges{
    MaxFlowEdge{0, 1, 3.0, 1},
    MaxFlowEdge{0, 2, 2.0, 1},
    MaxFlowEdge{1, 2, 5.0, 1},
    MaxFlowEdge{1, 3, 2.0, 1},
    MaxFlowEdge{2, 3, 3.0, 1}
  };

  milp_examples::solve_mincost_maxflow(nodes, edges, 0, 3, 5.0);
}