
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

std::string ClpWrapper::solveProblem(const std::string & problemFileOrContent)
{
    if (!readLp(problemFileOrContent))
    {
        return {};
    }

    primal();

    return getSolution(9);
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

std::string ClpWrapper::getSolution(const int precision) const
{
    using namespace std;
    const auto dim = _model->getNumCols();
    const auto solution = toFloatVector(_model->getColSolution(), dim);
    const auto unbounded = toFloatVector(_model->unboundedRay(), dim);
    const auto infeasibility = toFloatVector(_model->infeasibilityRay(), dim);

    std::vector<std::pair<std::string, std::string>> solutionObj;

    auto varNames = _model->columnNames();
    solutionObj.emplace_back("variables", asJsonArray(*varNames));
    solutionObj.emplace_back("solution", asJsonArray(solution, precision));
    solutionObj.emplace_back("unboundedRay", asJsonArray(unbounded, precision));
    solutionObj.emplace_back("infeasibilityRay", asJsonArray(infeasibility, precision));

    auto objectiveValue = _model->objectiveValue();
    solutionObj.emplace_back("objectiveValue", toString(objectiveValue, precision));

    auto solObjStr = asJsonObject(solutionObj);
    return solObjStr;
}
