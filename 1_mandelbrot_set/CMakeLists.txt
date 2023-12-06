cmake_minimum_required(VERSION 3.14)
project(mandelbrot_set)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
  example
  example.cpp
)