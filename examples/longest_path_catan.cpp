#include "milp_examples.hpp"

using namespace scip_wrapper;
using namespace milp_examples;



void milp_examples::longest_path_problem_catan(const std::vector<EdgePair> &edges,
                                const std::vector<fuint32_t> &nodes,
                                LongestPathResult& res)
{
  SCIPSolver solver{"longestPathSolver", SolverSense::MAXIMIZE};


  // create inflow and outflow variables (that act as s/t nodes)
  std::map<fuint32_t, fuint32_t> inflowVars{};
  std::map<fuint32_t, fuint32_t> outflowVars{};
  fuint32_t inflowConstraint = solver.createLinearConstraintEq(1.0);
  fuint32_t outflowConstraint = solver.createLinearConstraintEq(1.0);
  for (auto node : nodes)
  {
    fuint32_t inFlow = solver.createBinaryVar(0.0);
    fuint32_t outFlow = solver.createBinaryVar(0.0);
    solver.addToCst(inflowConstraint, inFlow);
    solver.addToCst(outflowConstraint, outFlow);

    inflowVars.insert(std::make_pair(node, inFlow));
    outflowVars.insert(std::make_pair(node, outFlow));
  }

  double q = 2 * edges.size(); // as a maximum the objective can become (needed for "ordering" constraints)


  std::map<fuint32_t, fuint32_t> flowConstraints{};
  std::map<fuint32_t, fuint32_t> orderVars{};
  for (auto node : nodes)
  {
    orderVars.insert(std::make_pair(node, solver.createIntVar(1.0, q, 0.0)));

    fuint32_t flowConstraint = solver.createLinearConstraintEq(0.0);
    flowConstraints.insert(std::make_pair(node, flowConstraint));

    solver.addToCst(flowConstraint, outflowVars[node], -1.0);
    solver.addToCst(flowConstraint, inflowVars[node],   1.0);
  }

  std::multimap<fuint32_t, std::pair<fuint32_t, fuint32_t>> edgeVars{};
  for (const auto &edge : edges)
  {
    fuint32_t edgeFromTo = solver.createBinaryVar(1.0);
    fuint32_t edgeToFrom = solver.createBinaryVar(1.0);

    // edge can only flow from one side, thus make constraint
    fuint32_t directionConstraint = solver.createLinearConstraintLeq(1.0);
    solver.addToCst(directionConstraint, edgeFromTo);
    solver.addToCst(directionConstraint, edgeToFrom);

    // add edges to flow constraints of both nodes
    fuint32_t fromNodeCst = flowConstraints[edge.first];
    fuint32_t toNodeCst   = flowConstraints[edge.second];
    solver.addToCst(fromNodeCst, edgeFromTo, -1.0); // flows out of fromNode
    solver.addToCst(fromNodeCst, edgeToFrom,  1.0); // inflow
    solver.addToCst(toNodeCst,   edgeFromTo,  1.0); // inflow
    solver.addToCst(toNodeCst,   edgeToFrom, -1.0); // flows out of toNode

    // create order constraint
    fuint32_t fromNodeOrderVar = orderVars[edge.first];
    fuint32_t toNodeOrderVar = orderVars[edge.second];
    fuint32_t fromNodeOrderCst = solver.createLinearConstraintLeq(q - 1.0);

    // fromNodeOrderCst is order node that gets "activated" when edge from fromNode to toNode is active
    solver.addToCst(fromNodeOrderCst, toNodeOrderVar, -1.0);
    solver.addToCst(fromNodeOrderCst, fromNodeOrderVar,  1.0);
    solver.addToCst(fromNodeOrderCst, edgeFromTo, q);
    solver.addToCst(fromNodeOrderCst, inflowVars[edge.second], -2*q);
    solver.addToCst(fromNodeOrderCst, outflowVars[edge.second], -2*q);

    // same for toNodeOrderCst as this is symmetric
    fuint32_t toNodeOrderCst = solver.createLinearConstraintLeq(q - 1.0);
    solver.addToCst(toNodeOrderCst, fromNodeOrderVar, -1.0);
    solver.addToCst(toNodeOrderCst, toNodeOrderVar,  1.0);
    solver.addToCst(toNodeOrderCst, edgeToFrom, q);
    solver.addToCst(toNodeOrderCst, inflowVars[edge.first], -2*q);
    solver.addToCst(toNodeOrderCst, outflowVars[edge.first], -2*q);

    // insert variables
    // std::cout << ">>> " << edge.first << " <-> " << edge.second  << "       " << edgeFromTo << "  " << edgeToFrom << std::endl;
    edgeVars.insert(std::make_pair(edge.first, std::make_pair(edge.second, edgeFromTo)));
    edgeVars.insert(std::make_pair(edge.second, std::make_pair(edge.first,  edgeToFrom)));
  }

  if(!solver.solve())
  {
    std::cerr << "Solving failed... Problem probably infeasible (which should never be the case for this problem)." << std::endl;
    return;
  }

  fuint32_t length = 0;
  for (const auto &edgeVar : edgeVars)
  {
    // std::cout << edgeVar.first << " " << edgeVar.second.first << " " << edgeVar.second.second << "  --->  " << solver.getVariableValue(edgeVar.second.second) << std::endl;
    if (solver.getBinaryValue(edgeVar.second.second))
    {
      res.edgesContained.insert(std::make_pair(edgeVar.first, edgeVar.second.first));
      std::cout << "Edge " << edgeVar.first << "------>" << edgeVar.second.first << std::endl;
      length++;
    }
  }

  std::cout << "Outputting order variables" << std::endl;
  for (auto node : nodes)
  {
    std::cout << "Node " << node << " has order value " << orderVars[node] << "." << std::endl;
  }

  for (auto node : nodes)
  {
    if(!solver.getBinaryValue(inflowVars[node])) continue;
    res.startNode = node;
    std::cout << "Tour starts at node " << node << "." << std::endl;
    break;
  }

  for (auto node : nodes)
  {
    if(!solver.getBinaryValue(outflowVars[node])) continue;
    res.endNode = node;
    std::cout << "Tour ends at node   " << node << "." << std::endl;
    break;
  }

  res.length = length;

  std::cout << std::endl
            << "Total length is " << length << std::endl;
}

namespace
{

  void printPath(const LongestPathResult& res)
  {
    std::multimap<fuint32_t, fuint32_t> edgesRemaining{res.edgesContained};

    fuint32_t currentNode = res.startNode;
    std::cout << std::endl << "Printing path:" << std::endl
              << currentNode;
    while (!edgesRemaining.empty())
    {
      auto it = edgesRemaining.find(currentNode);
      if (it == edgesRemaining.end())
      {
        std::cerr << "BUG..." << std::endl;
        return;
      }
      currentNode = it->second;
      std::cout << "-->" << currentNode;
      edgesRemaining.erase(it); // remove edge
    }
    std::cout << std::endl;
  }

  void minimalTest()
  {
    std::vector<fuint32_t> nodes
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8
    };

    std::vector<EdgePair> edges
    {
      /* The graph looks as follows
       *  0---1---2---3---4---5
       *              |      /
       *              8-----6----7
       *
       * Optimal solution has length 8.
       * The tour starts at 0 goes to 5 and ends at 3 (or reverse)
       */
      EdgePair(0,1),
      EdgePair(1,2),
      EdgePair(2,3),
      EdgePair(3,4),
      EdgePair(4,5),
      EdgePair(5,6),
      EdgePair(6,7),
      EdgePair(6,8),
      EdgePair(8,3)
    };
    LongestPathResult res{};

    milp_examples::longest_path_problem_catan(edges, nodes, res);
    printPath(res);
  }

  void bigTest()
  {

    /* The graph looks as follows
     *                  1
     *                /   \
     *               2     3
     *               |     |
     *               4     5
     *             /   \ /   \
     *            7     6     8
     *            |     |     |
     *            9    10    11
     *             \   / \   /
     *              12     13
     *               |     |
     *              14     15
     *                \   /
     *                 16
     *
     * I am not sure about the optimal solution.
     * The MILP finds a solution with all edges except for
     * (4 <===> 6) and (10 <===> 13)
     * The optimal length according to the MILP is 17.
     * I did not find a better solution manually.
     */
    std::vector<fuint32_t> nodes
    {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
    };

    std::vector<EdgePair> edges
    {
      EdgePair(1,2),
      EdgePair(1,3),
      EdgePair(2,4),
      EdgePair(3,5),
      EdgePair(4,7),
      EdgePair(4,6),
      EdgePair(5,6),
      EdgePair(5,8),
      EdgePair(7,9),
      EdgePair(6,10),
      EdgePair(8,11),
      EdgePair(9,12),
      EdgePair(10,12),
      EdgePair(10,13),
      EdgePair(11,13),
      EdgePair(12,14),
      EdgePair(13,15),
      EdgePair(14,16),
      EdgePair(15,16)
    };

    LongestPathResult res{};

    milp_examples::longest_path_problem_catan(edges, nodes, res);
    printPath(res);
  }
}

int main()
{
  //minimalTest();
  bigTest();
}
