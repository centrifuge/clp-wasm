#include "ClpWrapper.h"
#include "floatdef.h"

#include <boost/multiprecision/number.hpp>

std::string solve(std::string problem, int precision)
{
    ClpWrapper clpWrapper;
    return clpWrapper.solve(problem, precision);
}

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <sstream>

std::string bnRound(const std::string x)
{
    return mp::round(FloatT(x)).str();
}
std::string bnCeil(const std::string & x)
{
    return mp::ceil(FloatT(x)).str();
}
std::string bnFloor(const std::string & x)
{
    return mp::floor(FloatT(x)).str();
}
std::string version()
{
    return std::string(CLP_VERSION);
}

EMSCRIPTEN_BINDINGS(solver)
{
    using namespace emscripten;

    function("bnCeil", &bnCeil);
    function("bnRound", &bnRound);
    function("bnFloor", &bnFloor);
    function("solve", &solve);
    function("version", &version);

    class_<ClpWrapper>("ClpWrapper")
        .constructor<>()
        .function("solve", &ClpWrapper::solve)
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
        auto solution = solve(problemFile, 9);
        std::cout << "Problem: " << problemFile << std::endl;
        std::cout << "Solution: " << solution << std::endl;
    }
    return 0;
}
#endif
