#pragma once

#include <stddef.h>
#include <stdlib.h>

#include <RtAudio.h>

#include "utils.h"

namespace Engine {

struct InputManager
{
    InputManager();
    ~InputManager();

    void activate();

    inline bool active() const
    {
        return activated;
    }

    void processAudio(double* input);
    void readInput(double* buffer);

private:
    const Utils* utils;

    double* input = nullptr;

    bool activated = false;

};

}
