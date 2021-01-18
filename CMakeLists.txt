cmake_minimum_required(VERSION 3.16)
set (CMAKE_CXX_STANDARD 17)
project(solver)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_CONFIG_LOWER)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_UNITY_BUILD ON)

# file(RELATIVE_PATH CMAKE_NINJA_OUTPUT_PATH_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
# message(STATUS CMAKE_NINJA_OUTPUT_PATH_PREFIX=${CMAKE_NINJA_OUTPUT_PATH_PREFIX})
# set(CMAKE_NINJA_OUTPUT_PATH_PREFIX ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    endif()
endif()

if(EMSCRIPTEN)
    set(EM_FLAGS "--bind -std=c++17 -s WASM=1 -s ASSERTIONS=2 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0 -s MODULARIZE=1")
    if (${BUILD_CONFIG_LOWER} MATCHES "debug")
        message("Building in debug mode")
        set(EM_FLAGS "${EM_FLAGS} -g --source-map-base http://127.0.0.1:5501/web/")
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
    message(STATUS "Compiling native C++")
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
file(GLOB CLP_APP_FILES "clp/app/*.cpp")
add_executable(clp ${CLP_APP_FILES})
target_link_libraries(clp ${Boost_LIBRARIES} libclp)

# Solver wrapper
add_executable(solver
    solver/bindings.cc
    solver/ClpWrapper.cc
    solver/ProblemLoader.cc
)
target_link_libraries(solver ${Boost_LIBRARIES} libclp) 

