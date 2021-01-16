#pragma once
#include "config.h"
#include <string>
#include <vector>

inline auto MinusInf = std::numeric_limits<double>::min();
inline auto PlusInf = std::numeric_limits<double>::max();

using FloatVector = std::vector<FloatT>;
using IntVector = std::vector<int>;

struct Bound
{
    FloatT lowerBound { MinusInf };
    FloatT upperBound { PlusInf };
    std::string name {};

    bool valid() const
    {
        return lowerBound <= upperBound;
    }
};

struct RowConstraint : Bound
{
    std::vector<FloatT> row;
};

struct VariableDefinition : Bound
{
};

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

    std::string runWithClp();

private:
    std::vector<VariableDefinition> _variables;
    std::vector<RowConstraint> _constraints;

    std::string _name;
    ObjectiveDirection _objDirection { UNSET };
    FloatVector _objective;
};
