#include "milp_examples.hpp"

using namespace scip_wrapper;
using namespace milp_examples;

void milp_examples::weighted_max_cut(const std::vector<fuint32_t>& nodes,
                                     const std::vector<weight_edge_t>& edges)
{
  SCIPSolver solver{"MaxCutSolver", SolverSense::MAXIMIZE};

  // for each node there is a binary var denoting in which set of the partitioning that node is
  std::map<fuint32_t, fuint32_t> nodeVars{};
  for (auto node : nodes)
  {
    nodeVars.insert(std::make_pair(node, solver.createBinaryVar()));
  }

  // these are continuous vars (implicitly binary) that indicate
  // whether the edge is cut by the partitioning
  std::vector<fuint32_t> edgeVars{};
  edgeVars.reserve(edges.size());
  for (const auto &edge : edges)
  {
    // create constraint to check whether cut edge
    fuint32_t nodeAVar = nodeVars[edge.fromNode];
    fuint32_t nodeBVar = nodeVars[edge.toNode];
    fuint32_t helperVar = solver.createBinaryVar();
    fuint32_t edgeCutVar = solver.createVar(CONTINUOUS, 0.0, 1.0, edge.cost);

    fuint32_t edgeCutCst = solver.createLinearConstraintEq();
    solver.addToCst(edgeCutCst, nodeAVar);
    solver.addToCst(edgeCutCst, nodeBVar);
    solver.addToCst(edgeCutCst, edgeCutVar, -1.0);
    solver.addToCst(edgeCutCst, helperVar, -2.0);

    // nodeAVar + nodeBVar - edgeCutVar - 2 * helperVar = 0
    // this constraint enforces that if the edge is cut (thus, nodeAVar + nodeBVar = 1),
    // <=> edgeCutVar = 1 and helperVar = 0
    edgeVars.push_back(edgeCutVar);
  }

  if (!solver.solve())
  {
    std::cerr << "An error occurred." << std::endl;
  }

  std::cout << "Printing solution" << std::endl;
  double totalWeight = 0.0;
  for (fuint32_t i = 0; i < edges.size(); i++)
  {
    if (solver.getBinaryValue(edgeVars.at(i)))
    {
      const auto &edge = edges.at(i);
      std::cout << "Edge cut: " << edge.fromNode << "---[" << edge.cost << "]--->" << edge.toNode << std::endl;
      totalWeight += edge.cost;
    }
  }
  std::cout << "Total weight of max cut is " << totalWeight << "." << std::endl;

  std::cout << "Partitioning:" << std::endl;
  fuint32_t cardA = 0;
  for (auto node : nodes)
  {
    std::cout << "Node " << node << " is in set " << (solver.getBinaryValue(nodeVars[node]) ? "A" : "B") << std::endl;
    if (solver.getBinaryValue(nodeVars[node])) cardA++;
  }
  std::cout << "Cardinality of set A: " << cardA << " nodes." << std::endl;
  std::cout << "Cardinality of set B: " << (nodes.size() - cardA) << " nodes." << std::endl;
}

int main()
{
  /*    2-----4
   *  / |     |
   * 1  |     |
   *  \ |     |
   *    3-----5
   */
  std::vector<fuint32_t> nodes
  {
    1,2,3,4,5
  };

  std::vector<weight_edge_t> edges
  {
    weight_edge_t{1,2,1.0},
    weight_edge_t{1,3,1.0},
    weight_edge_t{2,3,1.0},
    weight_edge_t{2,4,1.0},
    weight_edge_t{3,5,1.0},
    weight_edge_t{4,5,1.0}
  };

  milp_examples::weighted_max_cut(nodes, edges);
}
