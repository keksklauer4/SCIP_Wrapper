#include "milp_examples.hpp"

using namespace scip_wrapper;
using namespace milp_examples;

/* keksklauer4 16.03.2021
 *
 * Dependency knapsack is the idea that in a knapsack with a given capacity
 * only items for which all items, the item depends on, are in the knapsack
 * can be included. If item A depends on item B and C, then A can only be
 * included if B and C are included.
 */

void milp_examples::solve_dependency_knapsack(const DependencyKnapsackItems &items,
                                 const DependencyVec &dependencies,
                                 double capacity)
{
  std::map<fuint32_t, fuint32_t> itemIndices{};

  MILPSolver solver{"DependencyKnapsack", SolverSense::MAXIMIZE};

  // create variables for items
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    fuint32_t varIndex = solver.createBinaryVar(it->weight, it->name.c_str());
    itemIndices.insert(std::make_pair(it->nodeID, varIndex));
  }

  // create capacity constraint
  fuint32_t capacityConstraint = solver.createLinearConstraintLeq(capacity);
  for (const auto& item : items)
  {
    fuint32_t varIndex = itemIndices[item.nodeID];
    solver.addToCst(capacityConstraint, varIndex, item.weight);
  }

  // create dependency constraints (item A can only be included if all items with edges from A are included)
  for (const auto& item : items)
  {
    if (item.nbEdges == 0) continue;
    // create constraint because dependencies exist
    fuint32_t depConstraint = solver.createLinearConstraintGeq(0.0);

    fuint32_t dependentVar = itemIndices[item.nodeID];
    solver.addToCst(depConstraint, dependentVar, -item.nbEdges);

    for (fuint32_t edgeIndex = item.edgeIndex; edgeIndex != item.edgeIndex + item.nbEdges; edgeIndex++)
    {
      fuint32_t varIndex = itemIndices[dependencies.at(edgeIndex)];
      solver.addToCst(depConstraint, varIndex);
    }
  }

  solver.solve();

  // Check solution
  std::cout << (solver.getSolution() != nullptr ? "Sucess!" : "An error occurred...") << std::endl;

  if (solver.getSolution() == nullptr) return;

  // Display items in knapsack:
  std::cout << "Items in the dependency knapsack:" << std::endl;
  double capacityUsed = 0.0;

  for (const auto &item : items)
  {
    fuint32_t varIndex = itemIndices[item.nodeID];
    if (solver.getBinaryValue(varIndex))
    {
      std::cout << "Item " << item.name << "(" << item.weight << ")" << std::endl;
      capacityUsed += item.weight;
    }
  }

  std::cout << std::endl
            << "Capacity used: " << capacityUsed << std::endl;
}


namespace
{
  void createDependencyKnapsackItem(DependencyKnapsackItems& items, DependencyVec& deps,
    fuint32_t nodeId, std::string name, double weight, std::vector<fuint32_t> dependencies)
  {
    items.push_back(DependencyKnapsackNode{name, nodeId, deps.size(), dependencies.size(), weight});
    for (auto dependency : dependencies)
    {
      deps.push_back(dependency);
    }
  }
}


int main()
{
  DependencyKnapsackItems items{};
  DependencyVec deps{};
  createDependencyKnapsackItem(items, deps, 0, "a", 12.0, {});
  createDependencyKnapsackItem(items, deps, 1, "b", 8.0,  {});
  createDependencyKnapsackItem(items, deps, 2, "c", 15.0, {});
  createDependencyKnapsackItem(items, deps, 3, "d", 10.0, {0,1}); // 3 depends on 0 and 1
  createDependencyKnapsackItem(items, deps, 4, "e", 3.0,  {0,2}); // 4 depends on 0 and 2

  milp_examples::solve_dependency_knapsack(items, deps, 33.0);
}
