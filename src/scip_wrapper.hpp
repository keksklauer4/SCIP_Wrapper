#ifndef SCIP_WRAPPER_HPP
#define SCIP_WRAPPER_HPP

// keksklauer4 16.03.2021

#include <cinttypes>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <exception>

// SCIP stuff
#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include "../external/scip_exception.hpp"

namespace scip_wrapper
{
  typedef uint_fast32_t fuint32_t;
  typedef SCIP_SOL SCIPSolution;
  typedef SCIP SCIPModel;
  enum SolverSense
  {
    MINIMIZE = +1,
    MAXIMIZE = -1
  };

  enum VariableType
  {
    BINARY,
    INTEGER,
    IMPLICIT_INTEGER,
    CONTINUOUS
  };


  struct SCIPVariable
  {
    SCIPVariable(){}
    SCIPVariable(SCIP_VAR* _var, void* _custom_data = nullptr)
      : variable(_var), custom(_custom_data) {}
    template <typename T>
    void setCustomData(T *data) { custom = reinterpret_cast<void *>(data); }

    public:
      SCIP_VAR *variable;
      void *custom;
  };

  struct SCIPLinearConstraint
  {
    SCIPLinearConstraint(){}
    SCIPLinearConstraint(SCIP_CONS* _cst, void* _custom_data = nullptr)
      : constraint(_cst), custom(_custom_data) {}
    void addVariable(SCIP* scip, SCIPVariable &var, double coefficient = 1.0);

    template <typename T>
    void setCustomData(T *data) { custom = reinterpret_cast<void *>(data); }

  public:
    SCIP_CONS *constraint;
    void *custom;
  };

  class SCIPSolver
  {
    public:
      SCIPSolver(std::string name, SolverSense sense);
      ~SCIPSolver();

    public:
      void solve();
      void setNbVars(fuint32_t nb);
      void setNbCsts(fuint32_t nb);

      fuint32_t createVar(VariableType type, double lower_bound, double upper_bound, double objective_coefficient = 0.0, const char *name = "");
      fuint32_t createVarUpperBounded(VariableType type, double upper_bound, double objective_coefficient = 0.0, const char *name = "");
      fuint32_t createVarLowerBounded(VariableType type, double lower_bound, double objective_coefficient = 0.0, const char *name = "");
      fuint32_t createBinaryVar(double objective_coefficient = 0.0, const char *name = "");

      fuint32_t createLinearConstraint(double lhs, double rhs, const char* name = "");
      fuint32_t createLinearConstraintEq(double equalVal = 0.0, const char* name = "");
      fuint32_t createLinearConstraintGeq(double lhs, const char *name = "");
      fuint32_t createLinearConstraintLeq(double rhs, const char *name = "");

      void addToCst(fuint32_t cstIndex, fuint32_t varIndex, double coefficient = 1.0);

      double getVariableValue(fuint32_t variableIndex);
      bool getBinaryValue(fuint32_t VariableIndex);
      SCIPSolution *getSolution() { return m_solution; }
      SCIPLinearConstraint *getCst(fuint32_t index) { return index < m_csts.size() ? &m_csts.at(index) : nullptr; }
      SCIPVariable *getVar(fuint32_t index) { return index < m_variables.size() ? &m_variables.at(index) : nullptr; }
      SCIPModel *getModel() { return m_scip_model; }
      void setLogging() { SCIP_CALL_EXC(SCIPsetIntParam(m_scip_model, "display/verblevel", 5)); }

    private:
      SCIPModel *m_scip_model;
      SCIPSolution *m_solution;
      std::vector<SCIPVariable> m_variables;
      std::vector<SCIPLinearConstraint> m_csts;
      std::string m_name;
  };


}


#endif
