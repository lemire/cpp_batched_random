cmake_minimum_required(VERSION 3.10)
project(cpp_batched_random)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Include directories
include_directories(include)
include_directories(benchmarks)


# Executables
add_executable(basic tests/basic.cpp)
add_executable(benchmark benchmarks/benchmark.cpp)

enable_testing()

add_test(NAME basic_test COMMAND basic)