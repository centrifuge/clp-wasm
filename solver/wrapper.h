#pragma once
#include <memory>
#include <string>

class ClpSimplex;

class ClpWrapper
{

public:

    ClpWrapper();
    std::string solveProblem(const std::string & problemFileOrContent);

private:


};