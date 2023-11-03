cmake_minimum_required(VERSION 3.21)
project(organic)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
add_executable(organic src/audiosource.cpp
                       src/config.cpp
                       src/effect.cpp
                       src/event.cpp
                       src/organic.cpp
                       src/parameter.cpp)
target_link_libraries(organic rtaudio)