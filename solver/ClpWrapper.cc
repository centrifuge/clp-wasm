
#include "ClpWrapper.h"
#include "ClpSimplex.hpp"

#include <fstream>
 
FILE * CbcOrClpReadCommand = stdin;
int CbcOrClpRead_mode = 1;

struct Membuf : std::streambuf
{
    Membuf(char const * base, const size_t size)
    {
        char * p(const_cast<char *>(base));
        this->setg(p, p, p + size);
    }
    ~Membuf() override = default;
};
struct Imemstream : virtual Membuf, std::istream
{
    Imemstream(char const * base, const size_t size)
    : Membuf(base, size)
    , std::istream(static_cast<std::streambuf *>(this))
    {
    }
};

std::string readContent(const std::string & problemFileOrContent)
{
    using namespace std;
    ifstream file(problemFileOrContent.c_str());
    string problemContent;
    if (file.good())
    {
        return string { istreambuf_iterator<char>(file), istreambuf_iterator<char>() };
    }
    return problemFileOrContent;
}

ClpWrapper::ClpWrapper()
: _model(std::make_shared<ClpSimplex>())
{
}

std::string ClpWrapper::solve(const std::string & problemFileOrContent, int precision)
{
    if (!readLp(problemFileOrContent))
    {
        return {};
    }

    primal();

    return getSolution(precision);
}

void ClpWrapper::primal()
{
    _model->createStatus();
    _model->primal();
}

void ClpWrapper::dual()
{
    _model->createStatus();
    _model->dual();
}

std::vector<FloatT> toFloatVector(const FloatT * data, int len)
{
    if (!data)
        return {};
    return std::vector<FloatT>(data, data + len);
}

bool ClpWrapper::readMps(const std::string & problemFileOrContent)
{
    return readInput(problemFileOrContent, ProblemFormat::LP);
}

bool ClpWrapper::readLp(const std::string & problemFileOrContent)
{
    return readInput(problemFileOrContent, ProblemFormat::LP);
}

bool ClpWrapper::readInput(const std::string & problemFileOrContent, ProblemFormat format)
{
    try
    {
        auto problemContent = readContent(problemFileOrContent);
        Imemstream stream(reinterpret_cast<char *>(&problemContent[0]), problemContent.size());
        if (format == ProblemFormat::LP)
            _model->readLp(stream);
        else if (format == ProblemFormat::MPS)
            // _model->readMps(stream);
            throw std::runtime_error("MPS format not supported just yet");
        return true;
    }
    catch (const std::exception & e)
    {
        return false;
    }
}

std::string toString(const FloatT & v, int precision)
{
    std::stringstream ss;
    ss << std::fixed;
    if (precision >= 0)
        ss << std::setprecision(std::max(1, precision));
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
    ss << std::fixed;
    if (precision >= 0)
        ss << std::setprecision(std::max(1, precision));
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
        string valWrapper = v.front() == '[' || v.front() == '{' ? "" : "\"";
        if (v == "true" || v == "false")
            valWrapper = "";
        ss << '"' << k << "\":" << valWrapper << v << valWrapper << (!isLast ? ',' : '}');
    }
    return ss.str();
}

bool findBestFloorCeilSolution(ClpSimplex * model)
{
    const auto dim = model->getNumCols();
    const auto originalSolution = toFloatVector(model->getColSolution(), dim);

    auto solution = originalSolution;
    auto ceilValues = solution;
    std::transform(ceilValues.begin(), ceilValues.end(), ceilValues.begin(), [](const FloatT & v) {
        const auto r = mp::round(v);
        if (mp::abs(r - v) > 1E-30)
            return mp::ceil(v);
        return r;
    });
    auto floorValues = solution;
    std::transform(floorValues.begin(), floorValues.end(), floorValues.begin(), [](const FloatT & v) {
        const auto r = mp::round(v);
        if (mp::abs(r - v) > 1E-30)
            return mp::floor(v);
        return r;
    });

    std::vector<FloatT> result {};
    FloatT best = 0;
    const auto N = 1 << dim;
    for (int n = 0; n < N; ++n)
    {
        for (int i = 0; i < dim; ++i)
        {
            solution[i] = (n & (1 << i)) ? ceilValues[i] : floorValues[i];
        }

        model->setColSolution(solution.data());
        model->checkSolution();
        if (model->numberPrimalInfeasibilities() == 0)
        {
            const auto objVal = model->objectiveValue() * model->optimizationDirection();
            if (result.empty() || objVal < best)
            {
                result = solution;
                best = objVal;
            }
        }
    }
    if (!result.empty())
    {
        model->setColSolution(result.data());
        model->checkSolution();
        return true;
    }
    else
    {
        model->setColSolution(originalSolution.data());
        model->checkSolution();
        return false;
    }
}

std::string ClpWrapper::getSolution(const int precision) const
{
    using namespace std;
    const auto dim = _model->getNumCols();

    bool integerSolution = false;
    if (precision <= 0 && dim <= 8)
    {
        integerSolution = findBestFloorCeilSolution(_model.get());
    }

    std::vector<std::pair<std::string, std::string>> solutionObj;

    auto varNames = _model->columnNames();
    const auto solution = toFloatVector(_model->getColSolution(), dim);
    const auto unbounded = toFloatVector(_model->unboundedRay(), dim);
    const auto infeasibility = toFloatVector(_model->infeasibilityRay(), dim);
    solutionObj.emplace_back("variables", asJsonArray(*varNames));
    solutionObj.emplace_back("solution", asJsonArray(solution, precision));
    solutionObj.emplace_back("unboundedRay", asJsonArray(unbounded, precision));
    solutionObj.emplace_back("infeasibilityRay", asJsonArray(infeasibility, precision));
    solutionObj.emplace_back("integerSolution", integerSolution ? "true" : "false");

    auto objectiveValue = _model->objectiveValue();
    solutionObj.emplace_back("objectiveValue", toString(objectiveValue, precision));

    auto solObjStr = asJsonObject(solutionObj);
    return solObjStr;
}
