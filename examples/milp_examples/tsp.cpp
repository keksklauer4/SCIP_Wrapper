#include "milp_examples.hpp"

using namespace scip_wrapper;
using namespace milp_examples;

/* keksklauer4 18.03.2021
 *
 * ILP example of the general TSP.
 * The formulation is called the Miller–Tucker–Zemlin formulation.
 */

void milp_examples::travelling_salesman_problem(const std::vector<fuint32_t> &nodes,
                                 const std::vector<EdgeWithCost> &edges,
                                 fuint32_t startNode)
{
  std::map<fuint32_t, fuint32_t> orderVars{};
  std::vector<fuint32_t> edgeVars{};
  // pair is <inflow, outflow>
  std::map<fuint32_t, std::pair<fuint32_t, fuint32_t>> nodeCsts{};

  double n = nodes.size();
  edgeVars.reserve(edges.size());

  MILPSolver solver{"TspSolver", SolverSense::MINIMIZE};

  for (auto node : nodes)
  {
    fuint32_t inflowCst  = solver.createLinearConstraintEq(1.0, "inflow");
    fuint32_t outflowCst = solver.createLinearConstraintEq(1.0, "outflow");
    nodeCsts.insert(std::make_pair(node, std::make_pair(inflowCst, outflowCst)));

    fuint32_t orderVar;
    if (node == startNode)
    { // order variable must be 1 for the start node.
      orderVar = solver.createVar(VariableType::CONTINUOUS, 1.0, 1.0, 0.0);
    }
    else
    {
      orderVar = solver.createIntVar(2.0, n, 0.0);
    }
    orderVars.insert(std::make_pair(node, orderVar));
  }

  for (const auto& edge : edges)
  {
    auto nodeFromCst = nodeCsts[edge.fromNode];
    auto nodeToCst = nodeCsts[edge.toNode];

    fuint32_t edgeVar = solver.createBinaryVar(edge.cost);
    solver.addToCst(nodeFromCst.first, edgeVar);
    solver.addToCst(nodeToCst.second, edgeVar);
    edgeVars.push_back(edgeVar);

    if (edge.toNode == startNode) continue; // no order constraint for last edge
    // now generate ordering constraint
    fuint32_t orderCst = solver.createLinearConstraintLeq(n - 1.0);
    fuint32_t nodeFromOrderVar = orderVars[edge.fromNode];
    fuint32_t nodeToOrderVar = orderVars[edge.toNode];
    solver.addToCst(orderCst, nodeToOrderVar, -1.0);
    solver.addToCst(orderCst, nodeFromOrderVar);
    solver.addToCst(orderCst, edgeVar, n);
  }

  if (!solver.solve())
  {
    std::cout << "Problem is probably infeasible." << std::endl;
  }

  std::vector<std::pair<fuint32_t, fuint32_t>> order{};
  order.reserve(orderVars.size());
  for (fuint32_t i = 0; i < n; i++)
  {
    order.push_back(std::make_pair(solver.getVariableValue(orderVars.at(i)), nodes.at(i)));
  }
  std::sort(order.begin(), order.end());

  for (auto it = order.begin(); it != order.end(); ++it)
  {
    std::cout << it->second;
    if (it + 1 == order.end()) continue;
    std::cout << "-->";
  }
  std::cout << std::endl;
  double totalDistance = 0.0;
  for (fuint32_t i = 0; i < edges.size(); i++)
  {
    if (solver.getBinaryValue(edgeVars.at(i)))
    {
      const auto& edge = edges.at(i);
      totalDistance += edge.cost;
      std::cout << "Edge " << edge.fromNode << "---[" << edge.cost
                << "]--->" << edge.toNode << std::endl;
    }
  }

  std::cout << "Total distance travelled: " << totalDistance << std::endl;
}

namespace
{
  const fuint32_t amountNodes = 6;
  std::vector<EdgeWithCost> createEdgeCostVec(const std::vector<fuint32_t>& nodes,
        const std::array<double, amountNodes * amountNodes> edgeCosts)
  {
    std::vector<EdgeWithCost> edges{};
    for (fuint32_t y = 0; y < amountNodes; y++)
    {
      fuint32_t nodeFrom = nodes.at(y);
      for (fuint32_t x = 0; x < amountNodes; x++)
      {
        double cost = edgeCosts[y * amountNodes + x];
        if (cost == 0.0) continue;
        fuint32_t nodeTo = nodes.at(x);
        edges.push_back(EdgeWithCost{nodeFrom, nodeTo, cost});
      }
    }
    return edges;
  }
}

int main()
{
  std::vector<fuint32_t> nodes{0, 1, 2, 3, 4, 5};
  std::array<double, amountNodes*amountNodes> edgeCosts =
  {0, 4, 3, 2, 4, 5,
   5, 0, 6, 3, 1, 3,
   6, 1, 0, 3, 4, 2,
   5, 3, 2, 0, 2, 5,
   3, 5, 7, 2, 0, 4,
   4, 6, 2, 4, 3, 0
  };
  auto edges = createEdgeCostVec(nodes, edgeCosts);
  milp_examples::travelling_salesman_problem(nodes, edges, 0);
}