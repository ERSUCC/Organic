#pragma once

#define _USE_MATH_DEFINES

#if defined(_WIN32)
    #include <math.h>
#else
    #include <cmath>
#endif

#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <string>

#include "location.h"

struct Utils
{
    static Utils* get();

    static void warning(const std::string message);

    static void parseWarning(const std::string message, const SourceLocation location);

    static void error(const std::string message);

    static void argumentError(const std::string message);
    static void parseError(const std::string message, const SourceLocation location);
    static void parseError(const std::string message, const std::string file, const unsigned int line, const unsigned int character);
    static void machineError(const std::string message, const std::string file);

    bool littleEndian = false;

    unsigned int sampleRate = 44100;
    unsigned int channels = 2;
    unsigned int bufferLength = 128;

    const double pi = M_PI;
    const double twoPi = M_PI * 2;
    const double infinity = std::numeric_limits<double>::infinity();

    double e;

    double volume = 0.25;
    double time = 0;

    std::mt19937 rng;

private:
    Utils();

    static Utils* instance;
    
};
