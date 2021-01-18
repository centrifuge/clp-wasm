#pragma once
#include "floatdef.h"
#include <string>
#include <vector>

class ClpSimplex;

inline FloatT MinusInf = -std::numeric_limits<double>::max();
inline FloatT PlusInf = std::numeric_limits<double>::max();

using FloatVector = std::vector<FloatT>;
using IntVector = std::vector<int>;

struct BoundedElement
{
    FloatT lowerBound { MinusInf };
    FloatT upperBound { PlusInf };
    std::string name {};
    bool valid() const
    {
        return lowerBound <= upperBound;
    }
};

struct RowConstraint : BoundedElement
{
    std::vector<FloatT> row;
};
using RowConstraintVector = std::vector<RowConstraint>;

struct VariableDefinition : BoundedElement
{
};
using VariableDefinitionVector = std::vector<VariableDefinition>;

enum ObjectiveDirection
{
    UNSET,
    MAXIMIZE,
    MINIMIZE
};

class ProblemLoader
{
public:
    void loadProblem(const std::string & problemFileOrContent);

    std::string getProblemName() const;

    const VariableDefinitionVector & getVariableDefinitions() const;

    const RowConstraintVector & getRowConstraints() const;

    const FloatVector & getObjective() const;

    ObjectiveDirection getObjectiveDirection() const;

    void setProblemOnModel(ClpSimplex & simplex);

    void reset();

    static bool checkIsCpplexProblem(const std::string & problemContent);

private:
    VariableDefinitionVector _variables;
    RowConstraintVector _constraints;

    std::string _problemName;
    ObjectiveDirection _objDirection { UNSET };
    FloatVector _objective;
};
