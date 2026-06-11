#pragma once

#define _USE_MATH_DEFINES

#if defined(_WIN32)
    #include <math.h>
#else
    #include <cmath>
#endif

#include <chrono>
#include <exception>
#include <iostream>
#include <limits>
#include <mutex>
#include <optional>
#include <random>
#include <string>

#include "exception.h"
#include "location.h"

struct Utils
{
    static Utils* get();

    static void printInfo();
    static void parseWarning(const std::string message, const SourceLocation& location);
    static void includeWarning(const std::string message, const SourceLocation& location);

    void setSeed(const std::optional<unsigned int>& seed);

    unsigned int channels;
    unsigned int sampleRate;
    unsigned int bufferLength;
    unsigned int seed;

    const double pi = M_PI;
    const double twoPi = M_PI * 2;
    const double e = M_E;
    const double infinity = std::numeric_limits<double>::infinity();

    double time = 0;

    std::mt19937 rng;

private:
    static Utils* instance;

    bool firstWarning = true;

};

struct Profiler
{
    Profiler(const double frequency);

    void start();
    void report();

private:
    void reset();

    static std::mutex lock;

    const double frequency;

    std::chrono::high_resolution_clock clock;

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> reportTime;

    std::chrono::duration<std::chrono::high_resolution_clock::rep, std::chrono::high_resolution_clock::period> elapsed;

    size_t count;

};
