#pragma once
#include "floatdef.h"
#include <memory>
#include <string>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
using namespace emscripten;
#endif

class ClpSimplex;
class ProblemLoader;

enum class ProblemFormat
{
    UNDEFINED,
    LP,
    MPS
};
class ClpWrapper
{
public:
    ClpWrapper();

    std::string solve(const std::string & problemFileOrContent, int precision);

public:
    bool readLp(const std::string & problemFileOrContent);

    bool readMps(const std::string & problemFileOrContent);

    void primal();

    void dual();

    std::string getSolution(const int precision) const;

    bool loadProblem(std::vector<FloatT> objec,
                     std::vector<FloatT> collb,
                     std::vector<FloatT> colub,
                     std::vector<FloatT> rowlb,
                     std::vector<FloatT> rowub,
                     std::vector<FloatT> matrix);

#ifdef __EMSCRIPTEN__
    bool loadProblemJS(val objec, val col_lb, val col_ub, val row_lb, val row_ub, val matrix);
    val getSolutionArray(int precision) const;
    val getUnboundedRay(int precision) const;
    val getInfeasibilityRay(int precision) const;
#endif

private:
    bool readInput(const std::string & problemFileOrContent, ProblemFormat format);

    std::shared_ptr<ClpSimplex> _model;
};