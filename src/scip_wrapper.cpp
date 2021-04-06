#include "scip_wrapper.hpp"

// keksklauer4 16.03.2021

#define INF SCIPinfinity(m_scip_model)
#define NEG_INF -SCIPinfinity(m_scip_model)

namespace scip_wrapper
{
  namespace
  {
    SCIP_OBJSENSE getSCIPObjSense(SolverSense sense)
    {
      switch(sense)
      {
        case SolverSense::MAXIMIZE: return SCIP_OBJSENSE_MAXIMIZE;
        case SolverSense::MINIMIZE: return SCIP_OBJSENSE_MINIMIZE;
      }
      return SCIP_OBJSENSE_MINIMIZE;
    }

    SCIP_Vartype getSCIPVarType(VariableType type)
    {
      #ifdef RELAX_PROBLEM
      // for testing purposes a quick way to just relax all variables to continuous variables
      return SCIP_Vartype::SCIP_VARTYPE_CONTINUOUS;
      #endif

      switch(type)
      {
        case VariableType::BINARY: return SCIP_Vartype::SCIP_VARTYPE_BINARY;
        case VariableType::INTEGER: return SCIP_Vartype::SCIP_VARTYPE_INTEGER;
        case VariableType::CONTINUOUS: return SCIP_Vartype::SCIP_VARTYPE_CONTINUOUS;
        case VariableType::IMPLICIT_INTEGER: return SCIP_Vartype::SCIP_VARTYPE_IMPLINT;
      }
      return SCIP_Vartype::SCIP_VARTYPE_CONTINUOUS;
    }
  }

template<>
void SCIPConstraint<true>::addVariable(SCIP* scip, SCIPVariable &var, double coefficient)
{
  SCIP_CALL_EXC(SCIPaddLinearVarQuadratic(scip, this->constraint, var.variable, coefficient));
}


template<>
void SCIPConstraint<false>::addVariable(SCIP* scip, SCIPVariable &var, double coefficient)
{
  SCIP_CALL_EXC(SCIPaddCoefLinear(scip, this->constraint, var.variable, coefficient));
}

template<>
void SCIPConstraint<true>::addQuadratic(SCIP *scip, SCIPVariable &var1, SCIPVariable &var2, double coefficient)
{
  SCIP_CALL_EXC(SCIPaddBilinTermQuadratic(scip, this->constraint, var1.variable, var2.variable, coefficient));
}


MILPSolver::MILPSolver(std::string name, SolverSense sense)
  : m_name(name)
{
  SCIP_CALL_EXC(SCIPcreate(&m_scip_model));
  SCIP_CALL_EXC(SCIPincludeDefaultPlugins(m_scip_model));
  SCIP_CALL_EXC(SCIPcreateProb(m_scip_model, m_name.c_str(), NULL, NULL,NULL, NULL, NULL, NULL, NULL));
  SCIP_CALL_EXC(SCIPsetObjsense(m_scip_model, getSCIPObjSense(sense)));
}

MILPSolver::~MILPSolver()
{
  try
  {
    if (m_scip_model == nullptr) return;

    for (auto it = m_variables.begin(); it != m_variables.end(); ++it)
    {
      SCIP_CALL_EXC(SCIPreleaseVar(m_scip_model, &it->variable));
    }
    for (auto it = m_csts.begin(); it != m_csts.end(); ++it)
    {
      SCIP_CALL_EXC(SCIPreleaseCons(m_scip_model, &it->constraint));
    }
    SCIP_CALL_EXC(SCIPfree(&m_scip_model));
  }
  catch(const std::exception& e)
  {
    std::cerr << "Error in destructor while trying to free SCIP variables/constraints/model: " << std::endl
              << e.what() << std::endl;
  }
}

bool MILPSolver::solve()
{
  SCIP_CALL_EXC(SCIPsolve(m_scip_model));
  m_solution = SCIPgetBestSol(m_scip_model);
  return m_solution != nullptr;
}

void MILPSolver::setNbVars(fuint32_t nb)
{
  if (m_variables.size() < nb) m_variables.reserve(nb);
}

void MILPSolver::setNbCsts(fuint32_t nb)
{
  if (m_csts.size() < nb) m_csts.reserve(nb);
}

double MILPSolver::getVariableValue(fuint32_t variableIndex)
{
  if (m_solution == nullptr) throw std::runtime_error("Solution is nullptr...");
  if (variableIndex >= m_variables.size()) throw std::range_error("Variable index out of range...");
  return SCIPgetSolVal(m_scip_model, m_solution, m_variables.at(variableIndex).variable);
}

bool MILPSolver::getBinaryValue(fuint32_t variableIndex)
{
  return getVariableValue(variableIndex) > 0.5;
}

fuint32_t MILPSolver::createVar(VariableType type, double lower_bound, double upper_bound, double objective_coefficient, const char *name)
{
  m_variables.push_back(SCIPVariable{});
  SCIP_CALL_EXC(SCIPcreateVar(m_scip_model, &m_variables.back().variable, name,
      lower_bound, upper_bound, objective_coefficient, getSCIPVarType(type), TRUE, FALSE,NULL, NULL, NULL, NULL, NULL));

  SCIP_CALL_EXC(SCIPaddVar(m_scip_model, m_variables.back().variable));
  return m_variables.size() - 1;
}

fuint32_t MILPSolver::createVarUpperBounded(VariableType type, double upper_bound, double objective_coefficient, const char *name)
{
  return createVar(type, NEG_INF, upper_bound, objective_coefficient, name);
}

fuint32_t MILPSolver::createVarLowerBounded(VariableType type, double lower_bound, double objective_coefficient, const char *name)
{
  return createVar(type, lower_bound, INF, objective_coefficient, name);
}

fuint32_t MILPSolver::createBinaryVar(double objective_coefficient, const char *name)
{
  return createVar(VariableType::BINARY, 0.0, 1.0, objective_coefficient, name);
}


fuint32_t MILPSolver::createIntVar(double lower_bound, double upper_bound, double objective_coefficient, const char *name)
{
  return createVar(VariableType::INTEGER, lower_bound, upper_bound, objective_coefficient, name);
}

fuint32_t MILPSolver::createIntVarUpperBounded(double upper_bound, double objective_coefficient, const char *name)
{
  return createVar(VariableType::INTEGER, NEG_INF, upper_bound, objective_coefficient, name);
}

fuint32_t MILPSolver::createIntVarLowerBounded(double lower_bound, double objective_coefficient, const char *name)
{
  return createVar(VariableType::INTEGER, lower_bound, INF, objective_coefficient, name);
}

fuint32_t MILPSolver::createLinearConstraint(double lhs, double rhs, const char* name)
{
  m_csts.push_back(SCIPLinearConstraint{});
  SCIP_CALL_EXC(SCIPcreateConsLinear(m_scip_model, &m_csts.back().constraint, name,
    0, NULL, NULL, lhs, rhs, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE));
  SCIP_CALL_EXC(SCIPaddCons(m_scip_model, m_csts.back().constraint));
  return m_csts.size() - 1;
}

fuint32_t MILPSolver::createLinearConstraintLeq(double rhs, const char* name)
{
  return createLinearConstraint(NEG_INF, rhs, name);
}

fuint32_t MILPSolver::createLinearConstraintGeq(double lhs, const char* name)
{
  return createLinearConstraint(lhs, INF, name);
}


fuint32_t MILPSolver::createLinearConstraintEq(double equalVal, const char* name)
{
  return createLinearConstraint(equalVal, equalVal, name);
}


void MILPSolver::createUnequalConstraint(fuint32_t x1, fuint32_t x2, double largeNb)
{ // only for integer/binary values
  // two constraints and one binary variable
  fuint32_t yVar = createBinaryVar(0.0);
  fuint32_t constraint1 = createLinearConstraintGeq(1.0);
  addToCst(constraint1, x1, 1.0);
  addToCst(constraint1, x2, -1.0);
  addToCst(constraint1, yVar, largeNb);

  fuint32_t constraint2 = createLinearConstraintLeq(largeNb - 1.0);
  addToCst(constraint2, x1, 1.0);
  addToCst(constraint2, x2, -1.0);
  addToCst(constraint2, yVar, largeNb);
}

void MILPSolver::addToCst(fuint32_t cstIndex, fuint32_t varIndex, double coefficient)
{
  if (m_csts.size() <= cstIndex || varIndex >= m_variables.size())
    throw std::range_error("Out of range in adding to constraint...");

  m_csts.at(cstIndex).addVariable(m_scip_model, m_variables.at(varIndex), coefficient);
}


/* ------------------------ QUBOSolver ------------------------ */

QUBOSolver::QUBOSolver(SolverSense sense, double penalty)
  : m_penalty(penalty), m_sense(sense), m_solution(nullptr)
{
  SCIP_CALL_EXC(SCIPcreate(&m_scip_model));
  SCIP_CALL_EXC(SCIPincludeDefaultPlugins(m_scip_model));
  SCIP_CALL_EXC(SCIPcreateProb(m_scip_model, "", NULL, NULL,NULL, NULL, NULL, NULL, NULL));
  SCIP_CALL_EXC(SCIPsetObjsense(m_scip_model, getSCIPObjSense(sense)));

  // create optimization variable
  SCIP_CALL_EXC(SCIPcreateVar(m_scip_model, &m_targetVar.variable, "target",
      NEG_INF, INF, 1.0, getSCIPVarType(CONTINUOUS), TRUE, FALSE,NULL, NULL, NULL, NULL, NULL));
  SCIP_CALL_EXC(SCIPaddVar(m_scip_model, m_targetVar.variable));

  // create constraint for optimization
  SCIP_CALL_EXC(SCIPcreateConsBasicQuadratic(m_scip_model, &m_targetConstraint.constraint, "cst",
    0, NULL, NULL, 0, NULL, NULL, NULL, m_sense == MINIMIZE ? NEG_INF : 0, m_sense == MINIMIZE ? 0 : INF));
  SCIP_CALL_EXC(SCIPaddCons(m_scip_model, m_targetConstraint.constraint));

  m_targetConstraint.addVariable(m_scip_model, m_targetVar, -1.0);
}

QUBOSolver::~QUBOSolver()
{
  try
  {
    if (m_scip_model == nullptr) return;

    for (auto it = m_variables.begin(); it != m_variables.end(); ++it)
    {
      SCIP_CALL_EXC(SCIPreleaseVar(m_scip_model, &it->variable));
    }
    if (m_targetVar.variable != nullptr)
    {
      SCIP_CALL_EXC(SCIPreleaseVar(m_scip_model, &m_targetVar.variable));
    }
    if (m_targetConstraint.constraint != nullptr)
    {
      SCIP_CALL_EXC(SCIPreleaseCons(m_scip_model, &m_targetConstraint.constraint));
    }
    SCIP_CALL_EXC(SCIPfree(&m_scip_model));
  }
  catch(const std::exception& e)
  {
    std::cerr << "Error in destructor while trying to free SCIP variables/constraints/model: " << std::endl
              << e.what() << std::endl;
  }
}


bool QUBOSolver::solve()
{
  if(m_solution != nullptr) return true;
  fillTargetConstraint();
  SCIP_CALL_EXC(SCIPsolve(m_scip_model));
  m_solution = SCIPgetBestSol(m_scip_model);
  return m_solution != nullptr;
}


void QUBOSolver::fillTargetConstraint()
{
  for (auto coefficientIt = m_coefficients.begin(); coefficientIt != m_coefficients.end(); ++coefficientIt)
  {
    auto& varPair = coefficientIt->first;
    auto coefficient = coefficientIt->second;
    if (varPair.first == varPair.second)
      m_targetConstraint.addVariable(m_scip_model, m_variables.at(varPair.first), coefficient);
    else
      m_targetConstraint.addQuadratic(m_scip_model, m_variables.at(varPair.first), m_variables.at(varPair.second), coefficient);
  }
}

fuint32_t QUBOSolver::createBinaryVar()
{
  m_variables.push_back(SCIPVariable{});
  SCIP_CALL_EXC(SCIPcreateVar(m_scip_model, &m_variables.back().variable, "",
      0.0, 1.0, 0.0, getSCIPVarType(BINARY), TRUE, FALSE,NULL, NULL, NULL, NULL, NULL));

  SCIP_CALL_EXC(SCIPaddVar(m_scip_model, m_variables.back().variable));
  return m_variables.size() - 1;
}

bool QUBOSolver::getBinaryValue(fuint32_t x)
{
  if (m_solution == nullptr) throw std::runtime_error("Solution is nullptr...");
  if (x >= m_variables.size()) throw std::range_error("Variable index out of range...");
  return SCIPgetSolVal(m_scip_model, m_solution, m_variables.at(x).variable) > 0.5;
}

void QUBOSolver::addQuadraticTerm(fuint32_t x, fuint32_t y, double coefficient, bool withPenalty)
{
  auto it = m_coefficients.find(VariablePair(std::min(x, y), std::max(x, y)));
  if (it == m_coefficients.end())
  {
    m_coefficients.insert(std::make_pair(VariablePair(std::min(x, y), std::max(x, y)),
      coefficient * (withPenalty ? m_penalty : 1.0)));
  }
  else
  {
    it->second += coefficient * (withPenalty ? m_penalty : 1.0);
  }
}

void QUBOSolver::addLinearTerm(fuint32_t x, double coefficient, bool withPenalty)
{
  addQuadraticTerm(x, x, coefficient, withPenalty);
}

void QUBOSolver::addImplicationTerm(fuint32_t x, fuint32_t y)
{

}

void QUBOSolver::addNotEqualTerm(fuint32_t x, fuint32_t y)
{

}

void QUBOSolver::addEqualityTerm(fuint32_t x, fuint32_t y)
{

}

void QUBOSolver::addAndTerm(fuint32_t x, fuint32_t y)
{

}

void QUBOSolver::addOrTerm(fuint32_t x, fuint32_t y)
{

}

void QUBOSolver::addLeqTerm(fuint32_t x, fuint32_t y)
{

}

void QUBOSolver::addGeqTerm(fuint32_t x, fuint32_t y)
{ // P(1 - x - y + x*y)
  addLinearTerm(x, -1.0, true);
  addLinearTerm(y, -1.0, true);
  addQuadraticTerm(x, y, 1.0, true);
}
}
