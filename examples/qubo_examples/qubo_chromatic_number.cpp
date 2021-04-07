#include "qubo_examples.hpp"

using namespace scip_wrapper;
using namespace qubo_examples;

/* keksklauer4 18.03.2021
 *
 * Calculating the chromatic number of a graph.
 */

void qubo_examples::chromatic_number(const std::vector<fuint32_t> &nodes,
                      const std::vector<EdgePair> &edges, fuint32_t k)
{
  QUBOSolver solver{SolverSense::MINIMIZE, 4.0};


  std::map<fuint32_t, std::vector<fuint32_t>> colorVars{};
  for (auto node : nodes)
  {
    std::vector<fuint32_t> vars{};
    vars.reserve(k);
    for (fuint32_t i = 0; i < k; i++)
    {
      vars.push_back(solver.createBinaryVar());
    }

    colorVars.insert(std::make_pair(node, vars));
  }
  for (auto node : nodes)
  {
    auto it = colorVars.find(node);
    for (auto varIt = it->second.begin(); varIt != it->second.end(); varIt++)
    {
      solver.addLinearTerm(*varIt, -1.0, true);
    }
    for (fuint32_t i = 0; i < k; i++)
    {
      for (fuint32_t j = i + 1; j < k; j++)
      {
        solver.addQuadraticTerm(it->second[i], it->second[j], 2.0, true);
      }
    }
  }

  for (const auto& edge : edges)
  {
    for (fuint32_t i = 0; i < k; i++)
    {
      fuint32_t colorVarFrom = colorVars[edge.first][i];
      fuint32_t colorVarTo = colorVars[edge.second][i];
      solver.addNandTerm(colorVarFrom, colorVarTo);
    }
  }

  std::cout << solver;

  if (!solver.solve())
  {
    std::cerr << "Something failed..." << std::endl;
    return;
  }

  for (auto it = colorVars.begin(); it != colorVars.end(); ++it)
  {
    for (fuint32_t i = 0; i < k; i++)
    {
      if (solver.getBinaryValue(it->second[i]))
      {
        std::cout << "Node " << it->first << " has color " << i << std::endl;
      }
    }
  }
}

namespace
{
  void simpleTest()
  {
    std::vector<fuint32_t> nodes
    {
      1,2,3,4,5
    };
    std::vector<EdgePair> edges
    {
      EdgePair(1,2),
      EdgePair(1,5),
      EdgePair(2,3),
      EdgePair(2,4),
      EdgePair(2,5),
      EdgePair(3,4),
      EdgePair(4,5),
    };
    qubo_examples::chromatic_number(nodes, edges, 3);
  }

  void complexTest()
  { // Example taken from Wikipedia: https://en.wikipedia.org/wiki/Graph_coloring
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
    qubo_examples::chromatic_number(nodes, edges, 3);
  }
}

int main()
{
  simpleTest();
  complexTest();
}