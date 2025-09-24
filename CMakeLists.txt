cmake_minimum_required(VERSION 3.10)
project(cpp_batched_random)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Include directories
include_directories(include)
include_directories(benchmarks)



option(BATCHED_RANDOM_SANITIZE "Sanitize addresses" OFF)
if(BATCHED_RANDOM_SANITIZE)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(STATUS "The address sanitizer under Apple's clang appears to be \
incompatible with the undefined-behavior sanitizer.")
    message(STATUS "You may set BATCHED_RANDOM_SANITIZE_UNDEFINED to sanitize \
undefined behavior.")
    add_compile_options(
        -fsanitize=address -fno-omit-frame-pointer -fno-sanitize-recover=all
    )
    add_compile_definitions(ASAN_OPTIONS=detect_leaks=1)
    link_libraries(
        -fsanitize=address -fno-omit-frame-pointer -fno-sanitize-recover=all
    )
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(-fsanitize=address)
    link_libraries(-fsanitize=address)
  else()
    message(
        STATUS
        "Setting both the address sanitizer and the undefined sanitizer."
    )
    add_compile_options(
        -fsanitize=address -fno-omit-frame-pointer
        -fsanitize=undefined -fno-sanitize-recover=all
    )
    link_libraries(
        -fsanitize=address -fno-omit-frame-pointer
        -fsanitize=undefined -fno-sanitize-recover=all
    )
  endif()
endif()


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES AND NOT BATCHED_RANDOM_SANITIZE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release)
endif()

# Executables
add_executable(basic tests/basic.cpp)
add_executable(benchmark benchmarks/benchmark.cpp)

enable_testing()

add_test(NAME basic_test COMMAND basic)