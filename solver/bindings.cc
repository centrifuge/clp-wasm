#include "ClpWrapper.h"

std::string solveLinearProblem(std::string problem)
{
    ClpWrapper clpWrapper;
    return clpWrapper.solveProblem(problem);
}

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <sstream>

namespace bm = boost::math;
std::string bn_round(std::string number)
{
    std::stringstream ss;
    ss << number;
    float_type x;
    ss >> x;
    return bm::round(x).str();
}

EMSCRIPTEN_BINDINGS(solver)
{
    using namespace emscripten;

    function("bn_round", &bn_round);
    function("solveLinearProblem", &solveLinearProblem);

    class_<ClpWrapper>("ClpWrapper").constructor<>().function("solveProblem", &ClpWrapper::solveProblem);
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
