
cmake_minimum_required(VERSION 2.6)
project(OneginSort)

set(CMAKE_CXX_FLAGS "-std=c++14")
add_executable(onegin main.cpp)
add_executable(tests test.cpp)
