/*
This file is part of C++lex, a project by Tommaso Urli.

C++lex is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

C++lex is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with C++lex.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "matrix.h"
#include "simplex.h"
#include <iostream>

using namespace pilal;
using namespace optimization;

int runSolverForFile(const std::string & problemFile)
{
    if (!problemFile.empty())
    {
        Simplex problem("TinLake");
        try
        {
            problem.load_problem(problemFile);

            // Solve
            problem.solve();
            std::cout << std::endl;

            if (problem.must_be_fixed())
            {
                std::cout << "Problem formulation is incorrect." << std::endl;
                return 1;
            }

            if (problem.has_solutions())
            {
                if (!problem.is_unlimited())
                    problem.print_solution();
                else
                    std::cout << "Problem is unlimited." << std::endl;
            }
            else
            {
                std::cout << "Problem is overconstrained." << std::endl;
            }
        }
        catch (DataMismatchException c)
        {
            std::cout << "Error: " << c.error << std::endl;
        }

        return 0;
    }
    else
    {
        std::cout << "Error: omitted problem file." << std::endl;
        return 1;
    }
}

#ifndef __EMSCRIPTEN__
int main(int argc, char * argv[])
{
    int rc = 0;
    for (int k = 1; k < argc; ++k)
    {
        const auto problemFile = std::string(argv[k]);
        rc = runSolverForFile(problemFile);
        if (rc != 0)
            break;
    }

    std::cout << "Quitting ..." << std::endl;
    return rc;
}
#endif