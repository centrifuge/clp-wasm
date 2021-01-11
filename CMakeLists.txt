cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 17)
project(cpplex)

#set(CMAKE_CXX_FLAGS "-std=c++11 -O3")

#add_subdirectory(pilal)
#add_subdirectory(simplex)
#include_directories(multiprecision/include)


find_package(Boost REQUIRED)

IF (Boost_FOUND)
    message(STATUS "BOOST FOUND !")
ELSE()
    message(STATUS "BOOST NOT Found !")
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(pilal/include)
include_directories(simplex/include)

message(STATUS Boost_LIBRARIES=${Boost_LIBRARIES})
 
add_executable(main

    solver/solver.cc

    simplex/src/columnset.cc 
    simplex/src/constraint.cc 
    simplex/src/objectivefunction.cc 
    simplex/src/simplex.cc 
    simplex/src/variable.cc
    simplex/src/matrix.cc

)
target_link_libraries(main ${Boost_LIBRARIES}) 

