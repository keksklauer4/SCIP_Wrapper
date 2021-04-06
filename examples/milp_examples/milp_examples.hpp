#ifndef MILP_EXAMPLES_HPP
#define MILP_EXAMPLES_HPP

#include "../example_types.hpp"

// just tests for milp solver stuff...
namespace milp_examples
{
  using namespace example_types;
  void solve_minimum_vertex_cover(std::vector<EdgePair> &edges,
                                  std::vector<double> weights);
  void solve_independent_set(const std::vector<EdgePair> &edges,
                             const std::vector<scip_wrapper::fuint32_t> nodes);


  void solve_dependency_knapsack(const DependencyKnapsackItems &nodes,
                                 const DependencyVec &dependencies,
                                 double capacity);

  void solve_n_queens_completion(fuint32_t n, const std::set<CoordinatePair>& placed_queens);



  void solve_mincost_maxflow(const NodeIDVec &nodes, const MaxFlowEdgeVec &edges,
        fuint32_t sNode, fuint32_t tNode, double targetFlow);


  void team_matching_problem(const std::vector<fuint32_t> &teams,
                             const std::vector<Member> &members,
                             const PreferenceVec &preferences);


  void longest_path_problem_catan(const std::vector<EdgePair> &edges,
                                  const std::vector<fuint32_t> &nodes,
                                  LongestPathResult& res);


  void travelling_salesman_problem(const std::vector<fuint32_t> &nodes,
                                   const std::vector<EdgeWithCost> &edges,
                                   fuint32_t startNode);

  void chromatic_number(const std::vector<fuint32_t> &nodes,
                        const std::vector<EdgePair> &edges);

  void three_partition(const std::vector<fuint32_t> &numbers);

  void max_clique(const std::vector<fuint32_t> &nodes,
                  const std::vector<EdgePair> &edges);

  void weighted_max_cut(const std::vector<fuint32_t> &nodes,
                        const std::vector<weight_edge_t> &edges);


  void rectangle_packing(std::vector<rectangle_t> &rects, double xLength, double yLength);

}

#endif
