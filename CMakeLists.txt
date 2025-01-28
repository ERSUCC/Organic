cmake_minimum_required(VERSION 3.21)

project(organic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# build main API as static library so it can be linked with the command line program and the test suite

add_library(organic_lib STATIC src/audiosource.cpp
                               src/bytecode.cpp
                               src/controller.cpp
                               src/effect.cpp
                               src/event.cpp
                               src/flags.cpp
                               src/location.cpp
                               src/machine.cpp
                               src/object.cpp
                               src/organic.cpp
                               src/parse.cpp
                               src/path.cpp
                               src/token.cpp
                               src/utils.cpp)

find_package(SndFile REQUIRED PATHS deps/libsndfile/cmake)

target_include_directories(organic_lib PRIVATE deps/rtaudio ${SNDFILE_INCLUDE_DIR})

add_library(rtaudio STATIC deps/rtaudio/RtAudio.cpp)

if (WIN32)
    target_compile_options(rtaudio PRIVATE -D__WINDOWS_WASAPI__)

    find_package(Threads REQUIRED CMAKE_THREAD_PREFER_PTHREAD THREADS_PREFER_PTHREAD_FLAG)

    target_link_libraries(rtaudio ksuser mfplat mfuuid wmcodecdspuuid winmm ole32 Threads::Threads)
endif()

if (APPLE)
    target_compile_options(rtaudio PRIVATE -D__MACOSX_CORE__)

    find_library(COREAUDIO_LIB CoreAudio)
    find_library(COREFOUNDATION_LIB CoreFoundation)

    target_link_libraries(rtaudio ${COREAUDIO_LIB} ${COREFOUNDATION_LIB})
endif()

if (LINUX)
    target_compile_options(rtaudio PRIVATE -D__LINUX_ALSA__)

    find_package(ALSA)

    find_package(Threads REQUIRED CMAKE_THREAD_PREFER_PTHREAD THREADS_PREFER_PTHREAD_FLAG)

    target_include_directories(rtaudio PRIVATE ${ALSA_INCLUDE_DIRS})
    target_link_libraries(rtaudio ${ALSA_LIBRARIES} Threads::Threads)
endif()

target_link_libraries(organic_lib rtaudio ${SNDFILE_LIBRARIES})

# build command line entry point

add_executable(organic src/main.cpp)

target_include_directories(organic PRIVATE deps/rtaudio ${SNDFILE_INCLUDE_DIR})

target_link_libraries(organic organic_lib)

# build test suite

add_executable(organic_test test/src/main.cpp
                            test/src/test.cpp)

target_compile_definitions(organic_test PRIVATE ORGANIC_TEST_DIR="${CMAKE_SOURCE_DIR}/test/files")

target_include_directories(organic_test PRIVATE include)

target_link_libraries(organic_test organic_lib)
