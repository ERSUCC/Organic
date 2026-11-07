#pragma once

#include <stddef.h>
#include <stdlib.h>

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

    void processAudio(const double* input);
    void readInput(double* buffer) const;

private:
    const Utils* utils;

    bool activated = false;

    double* input = nullptr;

};

}
