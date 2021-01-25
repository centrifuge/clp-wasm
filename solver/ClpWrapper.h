#pragma once
#include <memory>
#include <string>

class ClpSimplex;
class ProblemLoader;

enum class ProblemFormat {
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

private:
    bool readInput(const std::string & problemFileOrContent, ProblemFormat format);

    std::shared_ptr<ClpSimplex> _model;
};