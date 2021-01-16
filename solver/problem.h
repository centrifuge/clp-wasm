#pragma once

#include <string>
#include <vector>

using FloatType = double;
constexpr auto MinusInf = std::numeric_limits<FloatType>::min();
constexpr auto PlusInf = std::numeric_limits<FloatType>::max();

using FloatVector = std::vector<FloatType>;
using IntVector = std::vector<int>;

struct Bound
{
    FloatType lowerBound { MinusInf };
    FloatType upperBound { PlusInf };
    std::string name {};

    bool valid() const
    {
        return lowerBound <= upperBound;
    }
};

struct RowConstraint : Bound
{
    std::vector<FloatType> row;
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
