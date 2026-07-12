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
#include <stddef.h>
#include <string>

#include "exception.h"
#include "location.h"

enum struct WarnLevel
{
    Suppress,
    Display,
    Error
};

struct Utils
{
    static Utils* get();

    static void printInfo();
    static void setWarnLevel(const WarnLevel& level);
    static void parseWarning(const std::string& message, const SourceLocation& location);

    void setSeed(const std::optional<size_t>& seed);

    unsigned int channels;
    unsigned int sampleRate;
    unsigned int bufferLength;

    size_t seed;

    const double pi = M_PI;
    const double twoPi = M_PI * 2;
    const double e = M_E;
    const double infinity = std::numeric_limits<double>::infinity();

    double time = 0;
    double timeStep;

    std::mt19937_64 rng;

private:
    static Utils* instance;

    WarnLevel warnLevel = WarnLevel::Display;

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
