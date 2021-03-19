#ifndef VERTEX_COVER_HPP
#define VERTEX_COVER_HPP

#include <utility>
#include <map>
#include <set>
#include <array>
#include <algorithm>
#include <array>

#include "../src/scip_wrapper.hpp"

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


  typedef std::pair<fuint32_t, fuint32_t> CoordinatePair;
  void solve_n_queens_completion(fuint32_t n, const std::set<CoordinatePair>& placed_queens);

  struct MaxFlowEdge
  {
    MaxFlowEdge(fuint32_t _from, fuint32_t _to, double _capacity, double _cost)
      : fromNode(_from), toNode(_to), capacity(_capacity), cost(_cost){}

    public:
      fuint32_t fromNode;
      fuint32_t toNode;
      double capacity;
      double cost;
  };

  typedef std::vector<MaxFlowEdge> MaxFlowEdgeVec;
  typedef std::vector<fuint32_t> NodeIDVec;

  void solve_mincost_maxflow(const NodeIDVec &nodes, const MaxFlowEdgeVec &edges,
        fuint32_t sNode, fuint32_t tNode, double targetFlow);

  struct Member
  {
    Member(fuint32_t _id, std::string _name)
      :memberID(_id), name(_name){}
    fuint32_t memberID;
    std::string name;
  };
  struct MemberPreferences
  {
    MemberPreferences(fuint32_t _memberID, fuint32_t _teamID, double _cost)
      : memberID(_memberID), teamID(_teamID), cost(_cost) {}
    fuint32_t memberID;
    fuint32_t teamID;
    double cost; // how much would a person suffer if that person had to go to that team (lower is better)
  };

  typedef std::vector<MemberPreferences> PreferenceVec;
  void team_matching_problem(const std::vector<fuint32_t> &teams,
                             const std::vector<Member> &members,
                             const PreferenceVec &preferences);

  struct LongestPathResult
  {
    std::multimap<fuint32_t, fuint32_t> edgesContained;
    fuint32_t startNode;
    fuint32_t endNode;
    fuint32_t length;
  };

  void longest_path_problem_catan(const std::vector<EdgePair> &edges,
                                  const std::vector<fuint32_t> &nodes,
                                  LongestPathResult& res);

  struct EdgeWithCost
  {
    EdgeWithCost(fuint32_t _from, fuint32_t _to, double _cost)
      : fromNode(_from), toNode(_to), cost(_cost) {}
    fuint32_t fromNode;
    fuint32_t toNode;
    double cost;
  };

  void travelling_salesman_problem(const std::vector<fuint32_t> &nodes,
                                   const std::vector<EdgeWithCost> &edges,
                                   fuint32_t startNode);

  void chromatic_number(const std::vector<fuint32_t> &nodes,
                        const std::vector<EdgePair> &edges);

  void three_partition(const std::vector<fuint32_t> &numbers);

  void max_clique(const std::vector<fuint32_t> &nodes,
                  const std::vector<EdgePair> &edges);
}

#endif
