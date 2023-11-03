cmake_minimum_required(VERSION 3.21)

project(organic_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(organic_test src/test.cpp)

target_link_libraries(organic_test rtaudio)