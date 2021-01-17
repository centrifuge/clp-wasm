#ifdef __EMSCRIPTEN__
#include "ClpWrapper.h"

#include <emscripten/bind.h>
#include <sstream>

#include <boost/math/special_functions/round.hpp>
namespace bm = boost::math;

using namespace optimization;

std::string bn_round(std::string number)
{
    std::stringstream ss;
    ss << number;
    float_type x;
    ss >> x;
    return bm::round(x).str();
}

std::string solveLinarProblem(std::string problem)
{
    ClpWrapper clpWrapper;
    return clpWrapper.solveProblem(problem);
}

EMSCRIPTEN_BINDINGS(solver)
{
    using namespace emscripten;

    function("bn_round", &bn_round);

    class_<Simplex>("Simplex")
        .constructor<std::string>()
        .function("load_problem", &Simplex::load_problem)
        .function("solve", &Simplex::solve)
        .function("get_solution", &Simplex::get_solution)
        .function("is_unlimited", &Simplex::is_unlimited)
        .function("has_solutions", &Simplex::has_solutions);

    function("solveLinarProblem", &solveLinarProblem);

    class_<ClpWrapper>("ClpWrapper").constructor<std::string>().function("solveProblem", &ClpWrapper::solveProblem);
}
#endif
