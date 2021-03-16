#ifndef VERTEX_COVER_HPP
#define VERTEX_COVER_HPP

#include <utility>
#include <map>
#include <array>

#include "scip_wrapper.hpp"

// just tests for scip_wrapper stuff...
namespace milp_examples
{
  using scip_wrapper::fuint32_t;
  typedef std::pair<fuint32_t, fuint32_t> EdgePair;
  void solve_minimum_vertex_cover(std::vector<EdgePair> &edges,
                                  std::vector<double> weights);
  void solve_independent_set(const std::vector<EdgePair> &edges,
                             const std::vector<scip_wrapper::fuint32_t> nodes);

  struct DependencyKnapsackNode
  {
    DependencyKnapsackNode(std::string _name, fuint32_t _nodeID, fuint32_t _edgeIndex, fuint32_t _nbEdges, double _weight)
      : name(_name), nodeID(_nodeID), edgeIndex(_edgeIndex), nbEdges(_nbEdges), weight(_weight) {}

    std::string name;
    fuint32_t nodeID;
    fuint32_t edgeIndex;
    fuint32_t nbEdges;
    double weight;
  };

  typedef std::vector<DependencyKnapsackNode> DependencyKnapsackItems;
  typedef std::vector<fuint32_t> DependencyVec;

  void solve_dependency_knapsack(const DependencyKnapsackItems &nodes,
                                 const DependencyVec &dependencies,
                                 double capacity);

  void solve_n_queens(fuint32_t n);
}

#endif
