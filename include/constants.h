#pragma once

#include <math.h>

struct Constants
{
    static constexpr unsigned int SAMPLE_RATE = 44100;
    static constexpr unsigned int CHANNELS = 2;
    static constexpr unsigned int BUFFER_LENGTH = 256;
    static constexpr double TWO_PI = M_PI * 2;
};