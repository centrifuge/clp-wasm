// Loads problem from CPPLEX format into CLP model
#include "ClpSimplex.hpp"
#include "CoinPackedMatrix.hpp"

#include "ProblemLoader.h"
#include "memstream.h"

#include <fstream>
#include <sstream>

FloatT frStr(const std::string & val)
{
    const auto vLower = toLower(val);
    if (vLower == "-inf")
        return MinusInf;
    if (vLower == "inf" || vLower == "+inf")
        return PlusInf;
    if constexpr (std::is_same<double, FloatT>::value)
        return std::stod(val);
    else
        return FloatT(val);
}

enum ParsingContext
{
    PB_METADATA,
    PB_VARS,
    PB_CONSTRAINTS,
    PB_OBJECTIVE
};

bool ltSign(const std::string & token)
{
    return token == "<" || token == "<=";
}
bool gtSign(const std::string & token)
{
    return token == ">" || token == ">=";
}
bool eqSign(const std::string & token)
{
    return token == "=";
}

bool isCmpSign(const std::string & token)
{
    return ltSign(token) || gtSign(token) || eqSign(token);
}

void ProblemLoader::loadProblem(const std::string & problemFileOrContent)
{
    using namespace std;
    auto problem_content = problemFileOrContent;
    ifstream file(problemFileOrContent.c_str());
    if (file.good())
    {
        problem_content = string { istreambuf_iterator<char>(file), istreambuf_iterator<char>() };
    }
    Imemstream stream(reinterpret_cast<char *>(&problem_content[0]), problem_content.size());

    ParsingContext currentParsingBlock;
    int currentVar = 0, dimension = 0;

    while (stream.good())
    {
        string bufferInit, token;
        getline(stream, bufferInit);

        auto idx = bufferInit.find("//");
        if (idx != string::npos)
        {
            bufferInit.erase(idx);
        }

        stringstream buffer(bufferInit);

        std::vector<std::string> tokens;
        while (buffer.good())
        {
            std::string t;
            buffer >> t;
            tokens.push_back(std::move(t));
        }
        const auto nt = static_cast<int>(tokens.size());
        token = tokens[0];

        if (token.length())
        {
            if (token == "[METADATA]")
                currentParsingBlock = PB_METADATA;
            else if (token == "[VARIABLES]")
                currentParsingBlock = PB_VARS;
            else if (token == "[CONSTRAINTS]")
                currentParsingBlock = PB_CONSTRAINTS;
            else if (token == "[OBJECTIVE]")
                currentParsingBlock = PB_OBJECTIVE;
            else
            {
                switch (currentParsingBlock)
                {
                    case PB_METADATA:
                    {
                        if (token == "name")
                        {
                            bufferInit.erase(0, 5);
                            _problemName = bufferInit;
                        }
                        else if (token == "vars")
                        {
                            dimension = stoi(tokens[1]);
                        }
                    }
                    break;

                    case PB_VARS:
                    {
                        string variableName, lowerBound, upperBound;
                        if (tokens.size() == 3)
                        {
                            lowerBound = tokens[0];
                            variableName = tokens[1];
                            upperBound = tokens[2];
                        }
                        else if (tokens.size() == 5)
                        {
                            if (tokens[1] != tokens[3])
                                throw std::runtime_error("Should be same comparison type");

                            lowerBound = tokens[0];
                            variableName = tokens[2];
                            upperBound = tokens[4];
                        }

                        VariableDefinition var;
                        var.name = variableName;
                        var.lowerBound = frStr(lowerBound);
                        var.upperBound = frStr(upperBound);
                        _variables.push_back(std::move(var));
                        currentVar++;
                    }
                    break;

                    case PB_CONSTRAINTS:
                    {
                        std::string lowerBound = "-inf", upperBound = "inf";

                        if (currentVar != dimension)
                            throw std::runtime_error("Mismatch between declared and defined variables.");

                        if (nt != dimension + 2 && nt != dimension + 4)
                            throw std::runtime_error("Row constraints not specified correctly");

                        int ia = 0;
                        int ib = nt;

                        const auto & t0 = tokens[0];
                        const auto & t1 = tokens[1];
                        const auto & tp = tokens[nt - 2];
                        const auto & tn = tokens[nt - 1];

                        if (isCmpSign(t1))
                        {
                            ia = 2;
                            if (ltSign(t1))
                                lowerBound = t0;
                            else if (gtSign(t1))
                                upperBound = t0;
                            else if (eqSign(t1))
                            {
                                lowerBound = upperBound = t0;
                                if (nt != dimension + 2)
                                {
                                    throw std::runtime_error("Constraint is ill-formed");
                                }
                            }
                        }

                        if (isCmpSign(tp))
                        {
                            ib = nt - 2;
                            if (ltSign(tp))
                                upperBound = tn;
                            else if (gtSign(tp))
                                lowerBound = tn;
                            else if (eqSign(tp))
                            {
                                lowerBound = upperBound = tn;
                                if (nt != dimension + 2)
                                {
                                    throw std::runtime_error("Constraint is ill-formed");
                                }
                            }
                        }

                        RowConstraint c;
                        c.lowerBound = frStr(lowerBound);
                        c.upperBound = frStr(upperBound);

                        if (!c.valid())
                        {
                            throw std::runtime_error("Invalid bounds defined");
                        }

                        c.row.reserve(dimension);
                        if (ib - ia != dimension)
                            throw std::runtime_error("Constraint is ill-formed");

                        for (auto i = ia; i < ib; ++i)
                        {
                            c.row.push_back(frStr(tokens[i]));
                        }

                        _constraints.push_back(std::move(c));
                    }
                    break;

                    case PB_OBJECTIVE:
                    {
                        if (nt != dimension + 1)
                            throw std::runtime_error(
                                "Data mismatch error: Invalid number of tokens defining the objective function.");

                        if (token == "maximize")
                            _objDirection = MAXIMIZE;
                        else if (token == "minimize")
                            _objDirection = MINIMIZE;
                        else
                            throw std::runtime_error("Data mismatch error: Unknown objective function kind.");

                        _objective.clear();
                        _objective.reserve(dimension);
                        for (int i = 0; i < dimension; ++i)
                        {
                            _objective.push_back(frStr(tokens[i + 1]));
                        }
                    }
                    break;
                }
            }
        }
    }
}

std::string ProblemLoader::getProblemName() const
{
    return _problemName;
}

const VariableDefinitionVector & ProblemLoader::getVariableDefinitions() const
{
    return _variables;
}

const RowConstraintVector & ProblemLoader::getRowConstraints() const
{
    return _constraints;
}

const FloatVector & ProblemLoader::getObjective() const
{
    return _objective;
}

ObjectiveDirection ProblemLoader::getObjectiveDirection() const
{
    return _objDirection;
}

void ProblemLoader::setProblemOnModel(ClpSimplex & simplex)
{
    using namespace std;
    const auto dimension = _variables.size();
    FloatVector objective;
    objective.reserve(dimension);
    const auto sign = _objDirection == MAXIMIZE ? 1 : 1;
    for (const auto & v : _objective)
        objective.push_back(sign * v);

    FloatVector collb;
    FloatVector colub;
    FloatVector rowlb;
    FloatVector rowub;
    collb.reserve(dimension);
    colub.reserve(dimension);
    rowlb.reserve(dimension);
    rowub.reserve(dimension);

    std::vector<std::string> varNames;
    varNames.reserve(dimension);

    for (const auto & var : _variables)
    {
        collb.push_back(var.lowerBound);
        colub.push_back(var.upperBound);
        varNames.push_back(var.name);
    }

    IntVector rowIndices;
    IntVector colIndices;
    FloatVector matrixData;
    rowIndices.reserve(dimension);
    colIndices.reserve(dimension);
    matrixData.reserve(dimension);

    int r = 0;
    for (const auto & row : _constraints)
    {
        rowlb.push_back(row.lowerBound);
        rowub.push_back(row.upperBound);
        int c = 0;
        for (const auto & rv : row.row)
        {
            matrixData.push_back(rv);
            rowIndices.push_back(r);
            colIndices.push_back(c);
            c++;
        }
        r++;
    }

    const auto numElements = static_cast<int>(matrixData.size());
    const CoinPackedMatrix matrix(true, rowIndices.data(), colIndices.data(), matrixData.data(), numElements);
    simplex.setOptimizationDirection(_objDirection == MAXIMIZE ? -1.0 : 1.0);
    simplex.loadProblem(matrix, collb.data(), colub.data(), objective.data(), rowlb.data(), rowub.data());
    simplex.copyColumnNames(varNames, 0, dimension);
}

void ProblemLoader::reset()
{
    _objDirection = UNSET;
    _objective.clear();
    _variables.clear();
    _constraints.clear();
    _problemName.clear();
}

bool ProblemLoader::checkIsCpplexProblem(const std::string & problemContent)
{
    const std::vector<std::string> keywords = { "[METADATA]", "[VARIABLES]", "[CONSTRAINTS]", "[OBJECTIVE]"};
    return std::all_of(keywords.begin(), keywords.end(), [&problemContent](const std::string & keyword){
        return problemContent.find(keyword) != std::string::npos;
    });
}