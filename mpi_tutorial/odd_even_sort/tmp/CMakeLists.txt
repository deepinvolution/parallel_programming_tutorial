cmake_minimum_required(VERSION 3.14)
project(odd_even_sort)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  test_api
  test_api.cpp
)
add_executable(
  example
  example.cpp
)
target_link_libraries(
  test_api
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_api)