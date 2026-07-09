cmake_minimum_required(VERSION 3.21)

project(organic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# build main API as static library so it can be linked efficiently with the command line program and the test suite

add_library(organic_lib STATIC src/audiosource.cpp
                               src/controller.cpp
                               src/effect.cpp
                               src/exception.cpp
                               src/flags.cpp
                               src/location.cpp
                               src/object.cpp
                               src/organic.cpp
                               src/parse.cpp
                               src/path.cpp
                               src/program.cpp
                               src/resolve.cpp
                               src/resource.cpp
                               src/source.cpp
                               src/token.cpp
                               src/tokenize.cpp
                               src/transform.cpp
                               src/types.cpp
                               src/utils.cpp)

if (NOT WIN32)
    target_compile_options(organic_lib PRIVATE -O3)
endif ()

# link dependencies

set(BUILD_SHARED_LIBS True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(BUILD_TESTING False)

add_subdirectory(deps/rtaudio)
add_subdirectory(deps/libsndfile)
add_subdirectory(deps/libsamplerate)

target_include_directories(organic_lib PUBLIC deps/rtaudio deps/libsndfile/include deps/libsamplerate/include)

target_link_libraries(organic_lib rtaudio sndfile samplerate)

# build command line entry point

add_executable(organic src/main.cpp)

target_link_libraries(organic organic_lib)

# build test suite

add_executable(organic_test test/src/main.cpp
                            test/src/otest.cpp
                            test/src/test.cpp
                            test/src/test_examples.cpp
                            test/src/test_parser.cpp
                            test/src/test_resolver.cpp
                            test/src/test_tokenizer.cpp
                            test/src/test_utils.cpp
                            test/src/engine/test_controllers.cpp
                            test/src/engine/controllers/absolute.cpp
                            test/src/engine/controllers/add.cpp
                            test/src/engine/controllers/all.cpp
                            test/src/engine/controllers/any.cpp
                            test/src/engine/controllers/divide.cpp
                            test/src/engine/controllers/equals.cpp
                            test/src/engine/controllers/geq.cpp
                            test/src/engine/controllers/greater.cpp
                            test/src/engine/controllers/hold.cpp
                            test/src/engine/controllers/if.cpp
                            test/src/engine/controllers/leq.cpp
                            test/src/engine/controllers/less.cpp
                            test/src/engine/controllers/lfo.cpp
                            test/src/engine/controllers/limit.cpp
                            test/src/engine/controllers/max.cpp
                            test/src/engine/controllers/min.cpp
                            test/src/engine/controllers/multiply.cpp
                            test/src/engine/controllers/none.cpp
                            test/src/engine/controllers/power.cpp
                            test/src/engine/controllers/random.cpp
                            test/src/engine/controllers/repeat.cpp
                            test/src/engine/controllers/round.cpp
                            test/src/engine/controllers/sequence.cpp
                            test/src/engine/controllers/subtract.cpp
                            test/src/engine/controllers/sweep.cpp
                            test/src/engine/controllers/time.cpp
                            test/src/engine/controllers/trigger.cpp
                            test/src/engine/controllers/value.cpp)

target_compile_definitions(organic_test PRIVATE ORGANIC_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
                                                ORGANIC_TEST_DIR="${CMAKE_SOURCE_DIR}/test/files")

target_include_directories(organic_test PRIVATE include test/include)

target_link_libraries(organic_test organic_lib)
