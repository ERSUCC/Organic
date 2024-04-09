#pragma once

#define _USE_MATH_DEFINES

#if defined(_WIN32)
    #include <math.h>
#else
    #include <cmath>
#endif

#include <chrono>
#include <iostream>
#include <random>
#include <string>

struct Utils
{
    static Utils* get();

    static void warning(const std::string message);
    static void error(const std::string message);

    static void parseError(const std::string message, const std::string file, const unsigned int line, const unsigned int character);
    static void argumentError(const std::string message);
    static void machineError(const std::string message, const std::string file);

    unsigned int sampleRate = 44100;
    unsigned int channels = 2;
    unsigned int bufferLength = 128;

    const double twoPi = M_PI * 2;

    double volume = 0.25;
    double time = 0;

    std::mt19937 rng;

private:
    Utils();

    static Utils* instance;
    
};
