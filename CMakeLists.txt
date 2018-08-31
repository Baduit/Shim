cmake_minimum_required(VERSION 3.0)

project(ShimProject)

add_definitions(-std=c++17)

if(UNIX)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -pthread)
endif(UNIX)

set(SRCS
    src/main.cpp
    )

add_executable(shim ${SRCS})

target_link_libraries(shim replxx)
target_link_libraries(shim stdc++fs)

find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem)
include_directories(shim ${Boost_INCLUDE_DIRS})
link_directories(shim ${Boost_LIBRARY_DIRS})
target_link_libraries(shim ${Boost_LIBRARIES})

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/shim DESTINATION bin)