#include "milp_examples.hpp"

using namespace scip_wrapper;
using namespace milp_examples;


void milp_examples::max_clique(const std::vector<fuint32_t>& nodes,
                               const std::vector<EdgePair>& edges)
{
  MILPSolver solver{"MaxCliqueSolver", SolverSense::MAXIMIZE};

  fuint32_t amountNodesInCliqueVar = solver.createIntVarLowerBounded(0.0);
  fuint32_t amountNodesCst = solver.createLinearConstraintEq();
  solver.addToCst(amountNodesCst, amountNodesInCliqueVar);

  std::map<fuint32_t, fuint32_t> cliqueVars{};
  std::map<fuint32_t, fuint32_t> connectionCsts{};
  for (auto node : nodes)
  {
    fuint32_t cliqueVar = solver.createBinaryVar(1.0);
    cliqueVars.insert(std::make_pair(node, cliqueVar));
    solver.addToCst(amountNodesCst, cliqueVar, -1.0);

    fuint32_t connectionCst = solver.createLinearConstraintLeq(edges.size() + 1);
    solver.addToCst(connectionCst, amountNodesInCliqueVar);
    solver.addToCst(connectionCst, cliqueVar, edges.size());
    connectionCsts.insert(std::make_pair(node, connectionCst));
  }

  for (const auto& edge : edges)
  {
    fuint32_t nodeAClique = cliqueVars[edge.first];
    fuint32_t nodeBClique = cliqueVars[edge.second];
    fuint32_t connectionCstA = connectionCsts[edge.first];
    fuint32_t connectionCstB = connectionCsts[edge.second];

    solver.addToCst(connectionCstA, nodeBClique, -1.0);
    solver.addToCst(connectionCstB, nodeAClique, -1.0);
  }

  if (!solver.solve())
  {
    std::cerr << "Error occurred." << std::endl;
    return;
  }

  fuint32_t cliqueNb = 0;
  for (auto &cliqueVar : cliqueVars)
  {
    if (solver.getBinaryValue(cliqueVar.second))
    {
      std::cout << "Node " << cliqueVar.first << " is in max clique." << std::endl;
      cliqueNb++;
    }
  }

  std::cout << "Amount nodes in clique: " << cliqueNb << std::endl;
}

int main()
{
  std::vector<fuint32_t> nodes
  {
    0,1,2,3,4,5,6
  };

  std::vector<EdgePair> edges
  {
    EdgePair(0,1),
    EdgePair(0,2),
    EdgePair(0,3),
    EdgePair(0,5),

    EdgePair(1,2),
    EdgePair(1,4),
    EdgePair(1,6),

    EdgePair(2,3),
    EdgePair(2,4),

    EdgePair(3,4),
    EdgePair(3,5),
    EdgePair(3,6),
    EdgePair(4,5),
    EdgePair(4,6),
    EdgePair(5,6)
  };

  milp_examples::max_clique(nodes, edges);
}