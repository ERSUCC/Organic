#pragma once

#include <math.h>
#include <random>
#include <chrono>

struct Config
{
    static unsigned int SAMPLE_RATE;
    static unsigned int CHANNELS;
    static unsigned int BUFFER_LENGTH;
    static double TWO_PI;
    static double MASTER_VOLUME;
    static std::mt19937 RNG;
};