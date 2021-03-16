#include "vertex_cover.hpp"

using namespace scip_wrapper;

#define PAIR(a,b) std::make_pair(a,b)

int main()
{
  std::vector<double> weights
    { 1, 3, 3, 2, 2};
  std::vector<std::pair<fuint32_t, fuint32_t>> edges
  {
      PAIR(0,1),
      PAIR(0,3),
      PAIR(1,3),
      PAIR(1,2),
      PAIR(3,4),
      PAIR(2,3),
      PAIR(2,4)
  };

  vertex_cover::solve_minimum_vertex_cover(edges, weights);

}