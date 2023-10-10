#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <random>
#include <chrono>

struct Config
{
    static unsigned int SAMPLE_RATE;
    static unsigned int CHANNELS;
    static unsigned int BUFFER_LENGTH;
    static double TWO_PI;
    static double MASTER_VOLUME;
    static double TIME;
    static std::mt19937 RNG;
};