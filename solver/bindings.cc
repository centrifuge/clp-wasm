#ifndef __EMSCRIPTEN___
#include "simplex.h"
#include <emscripten/bind.h>

using namespace optimization;
EMSCRIPTEN_BINDINGS(solver)
{
    using namespace emscripten;

    class_<Simplex>("Simplex")
        .constructor<std::string>()
        .function("load_problem", &Simplex::load_problem)
        .function("solve", &Simplex::solve)
        .function("get_solution", &Simplex::get_solution)
        .function("is_unlimited", &Simplex::is_unlimited)
        .function("has_solutions", &Simplex::has_solutions);
}
#endif
