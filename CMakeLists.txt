cmake_minimum_required(VERSION 2.8)

project(cpplex)

set(CMAKE_CXX_FLAGS "-std=c++11 -O3")

#add_subdirectory(pilal)
#add_subdirectory(simplex)
#include_directories(multiprecision/include)
include_directories(pilal/include)
include_directories(simplex/include)

add_executable(main

    solver/solver.cc

    simplex/src/columnset.cc 
    simplex/src/constraint.cc 
    simplex/src/objectivefunction.cc 
    simplex/src/simplex.cc 
    simplex/src/variable.cc

    pilal/src/matrix.cc

)
target_link_libraries(main  ) 

