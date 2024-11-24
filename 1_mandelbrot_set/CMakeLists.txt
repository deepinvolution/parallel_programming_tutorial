cmake_minimum_required(VERSION 3.14)
project(mandelbrot_set)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
  example_omp example_omp.cpp
)
find_package(OpenMP)
target_link_libraries(example_omp PUBLIC png OpenMP::OpenMP_CXX)

function(add_program_target TARGET_NAME)
    add_executable(${TARGET_NAME} ${ARGN})
    target_compile_options(${TARGET_NAME} PUBLIC "-pthread")
    target_link_libraries(${TARGET_NAME} PUBLIC png)
endfunction()

add_program_target(example_pthread_no_opt example_pthread_no_opt.cpp)
add_program_target(example_pthread_thread_pool example_pthread_thread_pool.cpp)