#include "milp_examples.hpp"

#include <fstream>

using namespace scip_wrapper;
using namespace milp_examples;

const double sufficientlyLarge = 1000000.0;

namespace
{
  const std::string svgColors[] = {"red", "blue", "cyan", "purple", "green", "yellow", "pink", "gray", "orange", "darkgreen", "darkred"};
  fuint32_t createCst(SCIPSolver &solver, double length, fuint32_t xA, fuint32_t xB)
  {
    fuint32_t cstVar = solver.createBinaryVar(0.0);
    fuint32_t cstLowerCst = solver.createLinearConstraintLeq(length);
    fuint32_t cstUpperCst = solver.createLinearConstraintLeq(sufficientlyLarge - EPSILON - length);
    solver.addToCst(cstLowerCst, xB);
    solver.addToCst(cstLowerCst, xA, -1.0);
    solver.addToCst(cstLowerCst, cstVar, -sufficientlyLarge);

    solver.addToCst(cstUpperCst, xA);
    solver.addToCst(cstUpperCst, xB, -1.0);
    solver.addToCst(cstUpperCst, cstVar, sufficientlyLarge);
    return cstVar;
  }

  void createRandomSVGColor(std::ostream& os)
  { // too lazy to create random numbers from hex
    os << svgColors[rand() % 11];
  }
  void createSVGRect(std::ostream& os, double x, double y, double xLength, double yLength)
  {
    os << "<rect x=\"" << (x+5) << "\" width=\"" << (xLength-10) << "\" y=\"" << (y+5) << "\" height=\""
       << (yLength-10) << "\" stroke-width=\"10\" stroke=\"";
    createRandomSVGColor(os);
    os << "\"></rect>";
  }

  void createSVGText(std::ostream& os, const rectangle_t& rect)
  {
    os << "<text class=\"rectName\" x=\"" << (rect.x + rect.x_length / 2.0) << "\" y=\""
       << (rect.y + rect.y_length / 2.0) << "\">" << rect.name << "</text>";
  }

  void createSVGFile(std::string filename, const std::vector<rectangle_t>& rects, double xLength, double yLength)
  {
    std::ofstream file;
    file.open(filename);
    file << "<svg height=\"" << yLength << "\" width=\"" << xLength << "\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 "
         << xLength << " " << yLength << "\">" << "<style> .rectName { font: bold 30px sans-serif; fill: red; }</style>";

    createSVGRect(file, 0, 0, xLength, yLength);
    for (const auto& rect : rects)
    {
      if (!rect.included) continue;
      createSVGRect(file, rect.x, rect.y, rect.x_length, rect.y_length);
      createSVGText(file, rect);
    }

    file << "</svg>";
    file.close();
  }
}

void milp_examples::rectangle_packing(std::vector<rectangle_t>& rects, double xLength, double yLength)
{
  SCIPSolver solver{"RectPacking", SolverSense::MINIMIZE};

  std::vector<std::pair<fuint32_t, fuint32_t>> coordVars{};
  std::vector<fuint32_t> includedVars{};
  coordVars.reserve(rects.size());
  includedVars.reserve(rects.size());

  for (fuint32_t i = 0; i < rects.size(); i++)
  {
    auto &rect = rects.at(i);
    fuint32_t x = solver.createVarLowerBounded(CONTINUOUS, 0.0, 0.0);
    fuint32_t y = solver.createVarLowerBounded(CONTINUOUS, 0.0, 0.0);
    fuint32_t includedVar = solver.createBinaryVar(rect.getArea());
    coordVars.push_back(std::make_pair(x, y));
    includedVars.push_back(includedVar);

    // create inside board constraints
    fuint32_t xInBoardCst = solver.createLinearConstraintLeq(xLength - rect.x_length);
    solver.addToCst(xInBoardCst, x);
    solver.addToCst(xInBoardCst, includedVar, -sufficientlyLarge);

    fuint32_t yInBoardCst = solver.createLinearConstraintLeq(yLength - rect.y_length);
    solver.addToCst(yInBoardCst, y);
    solver.addToCst(yInBoardCst, includedVar, -sufficientlyLarge);
  }

  // now create constraints to enforce rects not to overlap
  for (fuint32_t i = 0; i < rects.size(); i++)
  {
    auto &rectA = rects.at(i);
    fuint32_t xA = coordVars[i].first;
    fuint32_t yA = coordVars[i].second;
    for (fuint32_t j = i + 1; j < rects.size(); j++)
    {
      auto &rectB = rects.at(j);
      fuint32_t xB = coordVars[j].first;
      fuint32_t yB = coordVars[j].second;

      // constraint pair 1: check whether xB >= xA + xA.x_length
      fuint32_t cst1Var = createCst(solver, rectA.x_length, xA, xB);

      // constraint pair 2: check whether xA >= xB + xB.x_length
      fuint32_t cst2Var = createCst(solver, rectB.x_length, xB, xA);

      // constraint pair 3: check whether yB >= yA + yA.y_length
      fuint32_t cst3Var = createCst(solver, rectA.y_length, yA, yB);

      // constraint pair 4: check whether yA >= yB + yB.y_length
      fuint32_t cst4Var = createCst(solver, rectB.y_length, yB, yA);

      // if at least one of the cstVars is 1, then they are not overlapping
      // Thus, cst1Var + cst2Var + cst3Var + cst4Var >= 1
      //      (because we do not allow overlapping rects)
      fuint32_t nonOverlappingCst = solver.createLinearConstraintGeq(1.0);
      solver.addToCst(nonOverlappingCst, cst1Var);
      solver.addToCst(nonOverlappingCst, cst2Var);
      solver.addToCst(nonOverlappingCst, cst3Var);
      solver.addToCst(nonOverlappingCst, cst4Var);
    }
  }

  // now solve and return results
  if (!solver.solve())
  {
    std::cerr << "Error, something failed unexpectedly..." << std::endl;
  }

  double areaUsed = 0.0;
  for (fuint32_t i = 0; i < rects.size(); i++)
  {
    auto &rect = rects.at(i);
    rect.included = !solver.getBinaryValue(includedVars.at(i));
    if (rect.included)
    {
      rect.x = solver.getVariableValue(coordVars.at(i).first);
      rect.y = solver.getVariableValue(coordVars.at(i).second);
      areaUsed += rect.getArea();
    }
  }
  std::cout << "Total area used: " << areaUsed << std::endl;
  std::cout << "Thats " << (100 * areaUsed / (xLength * yLength)) << "% of the total area." << std::endl;
}

int main()
{
  const double WIDTH = 1000.0;
  const double HEIGHT = 1000.0;
  srand(0);
  std::vector<rectangle_t> rects
  {
    Rectangle{300, 400, "Rect1"},
    Rectangle{200, 200, "Rect2"},
    Rectangle{100, 300, "Rect3"},
    Rectangle{500, 200, "Rect4"},
    Rectangle{300, 300, "Rect5"},
    Rectangle{400, 400, "Rect6"},
    Rectangle{600, 400, "Rect7"},
    Rectangle{400, 300, "Rect8"},
    Rectangle{400, 200, "Rect9"},
    Rectangle{100, 300, "Rect10"}
  };
  milp_examples::rectangle_packing(rects, WIDTH, HEIGHT);
  createSVGFile("rectanglePackingTest.svg", rects, WIDTH, HEIGHT);
}
