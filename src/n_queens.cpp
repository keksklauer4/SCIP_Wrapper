#include "milp_examples.hpp"

using namespace scip_wrapper;
using namespace milp_examples;

/* keksklauer4 16.03.2021
 *
 * An example using the scip wrapper to solve n queens problem.
 * The problem is about the maximum queens one can place on a
 * chess board without them interfering with each other.
 *
 * Not a 100% sure whether everything is correct.
 */

void milp_examples::solve_n_queens(fuint32_t n)
{
  std::vector<fuint32_t> boardVars{};
  boardVars.resize(n * n);

  SCIPSolver solver{"NQueensSolver", SolverSense::MAXIMIZE};

  for (auto& pos : boardVars)
  {
    pos = solver.createBinaryVar(1.0);
  }

  // create horizontal constraints (in each row at most one queen)
  for (fuint32_t rowIndex = 0; rowIndex < n; rowIndex++)
  {
    fuint32_t rowConstraint = solver.createLinearConstraintLeq(1.0);
    for (fuint32_t columnIndex = 0; columnIndex < n; columnIndex++)
    {
      solver.addToCst(rowConstraint, boardVars.at(rowIndex * n + columnIndex));
    }
  }

  // create vertical constraints (in each column at most one queen)
  for (fuint32_t columnIndex = 0; columnIndex < n; columnIndex++)
  {
    fuint32_t columnConstraint = solver.createLinearConstraintLeq(1.0);
    for (fuint32_t rowIndex = 0; rowIndex < n; rowIndex++)
    {
      solver.addToCst(columnConstraint, boardVars.at(rowIndex * n + columnIndex));
    }
  }

  // create diagonal constraints (upper left to lower right)
  int start = (-static_cast<int>(n)) + 2;
  int end = n - 1;
  for (int col = start; col < end; col++)
  {
    fuint32_t constraint = solver.createLinearConstraintLeq(1.0);
    for (int index = 0; index < n; index++)
    {
      if (col + index >= n || col + index < 0) continue;
      int pos = (col + index) * n + index;

      fuint32_t varIndex = boardVars.at(pos);
      solver.addToCst(constraint, varIndex);
    }
  }

  // create diagonal constraints (upper right to lower left)
  start = 1;
  end = 2 * n - 2;
  for (int col = start; col < end; col++)
  {
    fuint32_t constraint = solver.createLinearConstraintLeq(1.0);
    for (int index = n-1; index >= 0; index--)
    {
      if (col - index >= n || col - index < 0) continue;
      int pos = (col - index) * n + index;

      fuint32_t varIndex = boardVars.at(pos);
      solver.addToCst(constraint, varIndex);
    }
  }

  solver.solve();

  // Check solution
  std::cout << (solver.getSolution() != nullptr ? "Sucess!" : "An error occurred...") << std::endl;

  if (solver.getSolution() == nullptr) return;

  // Output solution:
  fuint32_t amountQueens = 0;
  for (fuint32_t rowIndex = 0; rowIndex < n; rowIndex++)
  {
    for (fuint32_t columnIndex = 0; columnIndex < n; columnIndex++)
    {
      if (solver.getBinaryValue(boardVars.at(rowIndex * n + columnIndex)))
      {
        std::cout << "X";
        amountQueens++;
      }
      else
      {
        std::cout << "O";
      }
    }
    std::cout << std::endl;
  }

  std::cout << std::endl
            << "Amount queens is " << amountQueens << "." << std::endl;
}


int main()
{
  milp_examples::solve_n_queens(8);
}

