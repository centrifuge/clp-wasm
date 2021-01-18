cmake_minimum_required(VERSION 3.12)
set (CMAKE_CXX_STANDARD 17)
project(solver)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_CONFIG_LOWER)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# file(RELATIVE_PATH CMAKE_NINJA_OUTPUT_PATH_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
# message(STATUS CMAKE_NINJA_OUTPUT_PATH_PREFIX=${CMAKE_NINJA_OUTPUT_PATH_PREFIX})
# set(CMAKE_NINJA_OUTPUT_PATH_PREFIX ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(EMSCRIPTEN)
    set(EM_FLAGS "--bind -std=c++17 -s WASM=1 -s ASSERTIONS=2 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0")
    if (${BUILD_CONFIG_LOWER} MATCHES "debug")
        message("Building in debug mode")
        set(EM_FLAGS "${EM_FLAGS} -g --source-map-base http://127.0.0.1:58832/web/")
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
message(STATUS CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS})

include_directories($ENV{BOOST_ROOT})
include_directories(common)

# CLP library
add_definitions(-DHAVE_CONFIG_H -DCLP_BUILD -DCOINUTILS_BUILD -DCOIN_HAS_CLP)
include_directories(clp/src)
file(GLOB CLP_CXX_FILES "clp/src/*.cpp")
add_library(libclp ${CLP_CXX_FILES} )
target_link_libraries(libclp ${Boost_LIBRARIES} )

# CLP command line application
add_executable(clp clp/app/ClpMain.cpp clp/app/ClpSolver.cpp clp/app/unitTest.cpp )
target_link_libraries(clp ${Boost_LIBRARIES} libclp)

# Solver wrapper
add_executable(solver
    solver/bindings.cc
    solver/ClpWrapper.cc
    solver/ProblemLoader.cc
)
target_link_libraries(solver ${Boost_LIBRARIES} libclp) 

