cmake_minimum_required(VERSION 3.21)

project(rtaudio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(rtaudio SHARED RtAudio.cpp)

target_compile_definitions(rtaudio PRIVATE -DRTAUDIO_EXPORT)

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
