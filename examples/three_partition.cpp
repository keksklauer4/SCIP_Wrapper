#include "milp_examples.hpp"
#include <numeric>

using namespace scip_wrapper;
using namespace milp_examples;

/* keksklauer4 19.03.2021
 *
 * Solving the 3-Partition problem using an ILP.
 */

struct Triplet
{
  Triplet(fuint32_t _i, fuint32_t _j, fuint32_t _k)
    : i(_i), j(_j), k(_k){}
  fuint32_t i;
  fuint32_t j;
  fuint32_t k;
};

void milp_examples::three_partition(const std::vector<fuint32_t> &numbers)
{
  if (numbers.size() % 3 != 0)
  {
    std::cout << "No 3p can be found." << std::endl;
    return;
  }
  fuint32_t m = numbers.size() / 3;
  fuint32_t t = std::accumulate(numbers.begin(), numbers.end(), 0);
  if (t % m != 0)
  {
    std::cout << "No 3p can be found." << std::endl;
    return;
  }
  t /= m;

  MILPSolver solver{"3PartitionSolver", SolverSense::MAXIMIZE};

  std::vector<fuint32_t> numberCsts{};
  numberCsts.resize(numbers.size());

  fuint32_t index = 0;
  for (auto& numberCst : numberCsts)
  { // every number used exactly once
    numberCst = solver.createLinearConstraintEq(1.0);
  }

  fuint32_t n = numbers.size();
  std::map<fuint32_t, Triplet> triplets{};
  for (fuint32_t i = 0; i < n; i++)
  {
    for (fuint32_t j = i+1; j < n; j++)
    {
      for (fuint32_t k = j + 1; k < n; k++)
      {
        if (numbers.at(i)+numbers.at(j)+numbers.at(k) != t) continue;
        fuint32_t tripletVar = solver.createBinaryVar(1.0);
        solver.addToCst(numberCsts.at(i), tripletVar);
        solver.addToCst(numberCsts.at(j), tripletVar);
        solver.addToCst(numberCsts.at(k), tripletVar);
        triplets.insert(std::make_pair(tripletVar, Triplet{i, j, k}));
      }
    }
  }

  if (!solver.solve())
  {
    std::cout << "No 3P found..." << std::endl;
    return;
  }

  fuint32_t amountTriplets = 0;
  for (const auto &triplet : triplets)
  {
    if (solver.getBinaryValue(triplet.first))
    {
      std::cout << "Triplet [" << triplet.second.i << "," << triplet.second.j << "," << triplet.second.k << "]: "
                << numbers.at(triplet.second.i) << " + " << numbers.at(triplet.second.j) << " + "
                << numbers.at(triplet.second.k) << " = " << t << std::endl;
      amountTriplets++;
    }
  }
  std::cout << amountTriplets << " triplets found." << std::endl;
}

int main()
{
  std::vector<fuint32_t> numbers
  {
    20, 23, 25, 30, 49, 45, 27, 30, 30, 40, 22, 19
  };
  milp_examples::three_partition(numbers);
}
