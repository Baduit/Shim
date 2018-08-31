cmake_minimum_required(VERSION 3.0)

project(ShimProject)

add_definitions(-std=c++17 )


set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -pthread)

set(SRCS
    src/main.cpp
    )

add_executable(shim ${SRCS})

target_link_libraries(shim replxx)
target_link_libraries(shim stdc++fs)


install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/shim DESTINATION bin)