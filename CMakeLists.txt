cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 17)
project(cpplex)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_CONFIG_LOWER)

if(EMSCRIPTEN)
    set(EM_FLAGS "--bind -std=c++17 -s WASM=1 -s ASSERTIONS=2 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0")
    if (${BUILD_CONFIG_LOWER} MATCHES "debug")
        message("Building in debug mode")
        set(EM_FLAGS "${EM_FLAGS} -g --source-map-base http://127.0.0.1:5500/web/")
    else()
        message("Building in release mode")
        set(EM_FLAGS "${EM_FLAGS} -O3")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EM_FLAGS}")
    foreach(FLAG_VAR CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${FLAG_VAR} MATCHES "-g")
            string(REGEX REPLACE "-g" "-g4" ${FLAG_VAR} "${${FLAG_VAR}}")
        endif()
    endforeach()
else()
    message("Not using emscripten!")
endif()

include_directories($ENV{BOOST_ROOT})
include_directories(simplex/include)

message(STATUS CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS})
 
add_executable(cpplex

    solver/solver.cc
    solver/bindings.cc

    simplex/src/columnset.cc 
    simplex/src/constraint.cc 
    simplex/src/objectivefunction.cc 
    simplex/src/simplex.cc 
    simplex/src/variable.cc
    simplex/src/matrix.cc

)
target_link_libraries(cpplex ${Boost_LIBRARIES}) 

