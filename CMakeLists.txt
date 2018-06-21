cmake_minimum_required(VERSION 3.0)

INCLUDE(CMakeForceCompiler) 
CMAKE_FORCE_C_COMPILER(gcc GNU)
CMAKE_FORCE_CXX_COMPILER(g++ GNU)

project(ShimProject)

add_definitions(-std=c++17 )

set(SRCS
    src/main.cpp
    )

add_executable(Shim ${SRCS})
target_link_libraries(Shim replxx)
target_link_libraries(Shim stdc++fs)