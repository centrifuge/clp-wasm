#include "ClpWrapper.h"
#include "floatdef.h"

std::string solveLinearProblem(std::string problem)
{
    ClpWrapper clpWrapper;
    return clpWrapper.solveProblem(problem);
}

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <sstream>

std::string bnRound(std::string number)
{
    FloatT x(number);
    return round(x).str();
}

std::string getClpVersion()
{
    return std::string(CLP_VERSION);
}

EMSCRIPTEN_BINDINGS(solver)
{
    using namespace emscripten;

    function("bnRound", &bnRound);
    function("solveLinearProblem", &solveLinearProblem);
    function("getClpVersion", &getClpVersion);
    class_<ClpWrapper>("ClpWrapper")
        .constructor<>()
        .function("solveProblem", &ClpWrapper::solveProblem)
        .function("readLp", &ClpWrapper::readLp)
        .function("readMps", &ClpWrapper::readMps)
        .function("primal", &ClpWrapper::primal)
        .function("dual", &ClpWrapper::dual)
        .function("getSolution", &ClpWrapper::getSolution);
}

#else
#include <iostream>
int main(int argc, char * argv[])
{
    for (int k = 1; k < argc; ++k)
    {
        const auto problemFile = std::string(argv[k]);
        auto solution = solveLinearProblem(problemFile);
        std::cout << "Problem: " << problemFile << std::endl;
        std::cout << "Solution: " << solution << std::endl;
    }
    return 0;
}
#endif
