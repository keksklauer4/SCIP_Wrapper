#ifndef QUBO_EXAMPLES_HPP
#define QUBO_EXAMPLES_HPP

#include "../example_types.hpp"

namespace qubo_examples
{
  using namespace example_types;

  void solve_minimum_vertex_cover(std::vector<EdgePair> &edges,
                                  std::vector<double> weights);

  void chromatic_number(const std::vector<fuint32_t> &nodes,
                        const std::vector<EdgePair> &edges, fuint32_t k);
}

#endif