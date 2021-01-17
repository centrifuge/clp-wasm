
#include "ClpWrapper.h"
#include "ClpSimplex.hpp"
#include "matrix.h"

#include "ProblemLoader.h"
#include <memory>

ClpWrapper::ClpWrapper()
: _model(std::make_shared<ClpSimplex>())
{
}

FloatVector toFloatVector(const FloatT * data, int len)
{
    if (!data)
        return {};
    return FloatVector(data, data + len);
}

std::string ClpWrapper::solveProblem(const std::string & problemFileOrContent)
{
    _problemLoader = std::make_shared<ProblemLoader>();

    _problemLoader->loadProblem(problemFileOrContent);

    _problemLoader->setProblemOnModel(*_model);

    _model->createStatus();

    _model->primal();

    return prepareSolution();
}

std::string toString(const FloatT & v, int precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision);
    ss << v;
    return ss.str();
}

template <typename T>
std::string asJsonArray(const std::vector<T> & vec, int precision = 0)
{
    if (vec.empty())
        return "[]";
    using namespace std;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision);
    ss << "[";
    auto i = 0;
    for (const auto & v : vec)
    {
        const auto isLast = ++i == vec.size();
        ss << '"' << v << '"' << (!isLast ? ',' : ']');
    }
    return ss.str();
}

std::string asJsonObject(const std::vector<std::pair<std::string, std::string>> & keyValues)
{
    if (keyValues.empty())
        return "{}";
    using namespace std;
    std::stringstream ss;
    ss << "{";
    size_t i = 0;
    for (const auto & [k, v] : keyValues)
    {
        const auto isLast = ++i == keyValues.size();
        const string valWrapper = v.front() == '[' || v.front() == '{' ? "" : "\"";
        ss << '"' << k << "\":" << valWrapper << v << valWrapper << (!isLast ? ',' : '}');
    }
    return ss.str();
}

std::string ClpWrapper::prepareSolution() const
{

    const int precision = 2;

    using namespace std;
    const auto dim = _model->getNumCols();
    const auto solution = toFloatVector(_model->getColSolution(), dim);
    const auto unbounded = toFloatVector(_model->unboundedRay(), dim);
    const auto infeasibility = toFloatVector(_model->infeasibilityRay(), dim);

    std::vector<std::pair<std::string, std::string>> solutionObj;

    std::vector<std::string> varNames;
    for (const auto & var : _problemLoader->getVariableDefinitions())
    {
        varNames.push_back(var.name);
    }

    solutionObj.emplace_back("variables", asJsonArray(varNames));
    solutionObj.emplace_back("solution", asJsonArray(solution, precision));
    solutionObj.emplace_back("unboundedRay", asJsonArray(unbounded, precision));
    solutionObj.emplace_back("infeasibilityRay", asJsonArray(infeasibility, precision));

    auto objectiveValue = _model->objectiveValue();
    if (_problemLoader->getObjectiveDirection() == MAXIMIZE)
        objectiveValue *= -1;

    solutionObj.emplace_back("objectiveValue", toString(objectiveValue, precision));

    auto solObjStr = asJsonObject(solutionObj);
    return solObjStr;
}