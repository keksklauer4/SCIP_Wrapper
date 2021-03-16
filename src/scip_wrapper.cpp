#include "scip_wrapper.hpp"

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

void SCIPLinearConstraint::addVariable(SCIP* scip, SCIPVariable &var, double coefficient)
{
  SCIP_CALL_EXC(SCIPaddCoefLinear(scip, this->constraint, var.variable, coefficient));
}


SCIPSolver::SCIPSolver(std::string name, SolverSense sense)
  : m_name(name)
{
  SCIP_CALL_EXC(SCIPcreate(&m_scip_model));
  SCIP_CALL_EXC(SCIPincludeDefaultPlugins(m_scip_model));
  SCIP_CALL_EXC(SCIPcreateProb(m_scip_model, m_name.c_str(), NULL, NULL,NULL, NULL, NULL, NULL, NULL));
  SCIP_CALL_EXC(SCIPsetObjsense(m_scip_model, getSCIPObjSense(sense)));
}

SCIPSolver::~SCIPSolver()
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

void SCIPSolver::solve()
{
  SCIP_CALL_EXC(SCIPsolve(m_scip_model));
  m_solution = SCIPgetBestSol(m_scip_model);
}

void SCIPSolver::setNbVars(fuint32_t nb)
{
  if (m_variables.size() < nb) m_variables.reserve(nb);
}

void SCIPSolver::setNbCsts(fuint32_t nb)
{
  if (m_csts.size() < nb) m_csts.reserve(nb);
}

double SCIPSolver::getVariableValue(fuint32_t variableIndex)
{
  if (m_solution == nullptr) throw std::runtime_error("Solution is nullptr...");
  if (variableIndex >= m_variables.size()) throw std::range_error("Variable index out of range...");
  return SCIPgetSolVal(m_scip_model, m_solution, m_variables.at(variableIndex).variable);
}

fuint32_t SCIPSolver::createVar(VariableType type, double lower_bound, double upper_bound, double objective_coefficient, const char *name)
{
  m_variables.push_back(SCIPVariable{});
  SCIP_CALL_EXC(SCIPcreateVar(m_scip_model, &m_variables.back().variable, name,
      lower_bound, upper_bound, objective_coefficient, getSCIPVarType(type), TRUE, FALSE,NULL, NULL, NULL, NULL, NULL));

  SCIP_CALL_EXC(SCIPaddVar(m_scip_model, m_variables.back().variable));
  return m_variables.size() - 1;
}

fuint32_t SCIPSolver::createVarUpperBounded(VariableType type, double upper_bound, double objective_coefficient, const char *name)
{
  return createVar(type, NEG_INF, upper_bound, objective_coefficient, name);
}

fuint32_t SCIPSolver::createVarLowerBounded(VariableType type, double lower_bound, double objective_coefficient, const char *name)
{
  return createVar(type, lower_bound, INF, objective_coefficient, name);
}

fuint32_t SCIPSolver::createBinaryVar(double objective_coefficient, const char *name)
{
  return createVar(VariableType::BINARY, 0.0, 1.0, objective_coefficient, name);
}

fuint32_t SCIPSolver::createLinearConstraint(double lhs, double rhs, const char* name)
{
  m_csts.push_back(SCIPLinearConstraint{});
  SCIP_CALL_EXC(SCIPcreateConsLinear(m_scip_model, &m_csts.back().constraint, name,
    0, NULL, NULL, lhs, rhs, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE));
  SCIP_CALL_EXC(SCIPaddCons(m_scip_model, m_csts.back().constraint));
  return m_csts.size() - 1;
}

fuint32_t SCIPSolver::createLinearConstraintLeq(double rhs, const char* name)
{
  return createLinearConstraint(NEG_INF, rhs, name);
}

fuint32_t SCIPSolver::createLinearConstraintGeq(double lhs, const char* name)
{
  return createLinearConstraint(lhs, INF, name);
}


void SCIPSolver::addToCst(fuint32_t cstIndex, fuint32_t varIndex, double coefficient)
{
  if (m_csts.size() <= cstIndex || varIndex >= m_variables.size())
    throw std::range_error("Out of range in adding to constraint...");

  m_csts.at(cstIndex).addVariable(m_scip_model, m_variables.at(varIndex), coefficient);
}
}
