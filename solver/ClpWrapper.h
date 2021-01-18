#pragma once
#include <memory>
#include <string>

class ClpSimplex;
class ProblemLoader;

class ClpWrapper
{
public:
    ClpWrapper();
    std::string solveProblem(const std::string & problemFileOrContent);
    std::string prepareSolution() const;

private:
    std::shared_ptr<ClpSimplex> _model;
};