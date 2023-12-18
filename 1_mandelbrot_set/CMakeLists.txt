cmake_minimum_required(VERSION 3.14)
project(mandelbrot_set)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
  example_omp example_omp.cpp
)
find_package(OpenMP)
target_link_libraries(example_omp PUBLIC png OpenMP::OpenMP_CXX)

add_executable(
  example_pthread example_pthread.cpp
)
find_package(Threads)
target_compile_options(example_pthread PUBLIC "-pthread")
target_link_libraries(example_pthread PUBLIC png)