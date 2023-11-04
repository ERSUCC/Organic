#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <queue>
#include <random>

#include "../include/RtAudio.h"

#include "../include/audiosource.h"
#include "../include/compile.h"
#include "../include/config.h"
#include "../include/effect.h"
#include "../include/event.h"
#include "../include/parameter.h"

namespace Organic
{
    void warning(const std::string message);
    void error(const std::string message);
};