#ifndef VERTEX_COVER_HPP
#define VERTEX_COVER_HPP

#include <utility>

#include "scip_wrapper.hpp"

// just a test for scip_wrapper stuff...
namespace vertex_cover
{

  void solve_minimum_vertex_cover(std::vector<std::pair<scip_wrapper::fuint32_t, scip_wrapper::fuint32_t>> &edges,
                                  std::vector<double> weights);

}



#endif
