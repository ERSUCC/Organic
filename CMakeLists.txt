cmake_minimum_required(VERSION 3.21)

project(organic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# build main API as static library so it can be linked efficiently with the command line program and the test suite

add_library(organic_lib STATIC src/audiosource.cpp
                               src/bytecode.cpp
                               src/controller.cpp
                               src/effect.cpp
                               src/flags.cpp
                               src/location.cpp
                               src/machine.cpp
                               src/object.cpp
                               src/organic.cpp
                               src/parse.cpp
                               src/path.cpp
                               src/token.cpp
                               src/utils.cpp)

# link dependencies

set(BUILD_SHARED_LIBS True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(deps/rtaudio)
add_subdirectory(deps/libsndfile)

target_include_directories(organic_lib PUBLIC deps/rtaudio deps/libsndfile/include)

target_link_libraries(organic_lib rtaudio sndfile)

# build command line entry point

add_executable(organic src/main.cpp)

target_link_libraries(organic organic_lib)

# build test suite

add_executable(organic_test test/src/main.cpp
                            test/src/test.cpp)

target_compile_definitions(organic_test PRIVATE ORGANIC_TEST_DIR="${CMAKE_SOURCE_DIR}/test/files")

target_include_directories(organic_test PRIVATE include)

target_link_libraries(organic_test organic_lib)
