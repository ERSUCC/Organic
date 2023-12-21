cmake_minimum_required(VERSION 3.21)

project(organic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(organic src/audiosource.cpp
                       src/effect.cpp
                       src/event.cpp
                       src/interpret.cpp
                       src/main.cpp
                       src/object.cpp
                       src/organic.cpp
                       src/parameter.cpp
                       src/parse.cpp
                       src/token.cpp
                       src/utils.cpp)

add_library(rtaudio STATIC src/RtAudio.cpp)

target_compile_options(rtaudio PRIVATE -D__MACOSX_CORE__)

find_library(COREAUDIO_LIB CoreAudio)
find_library(COREFOUNDATION_LIB CoreFoundation)

target_link_libraries(rtaudio ${COREAUDIO_LIB} ${COREFOUNDATION_LIB})

target_link_libraries(organic rtaudio)
