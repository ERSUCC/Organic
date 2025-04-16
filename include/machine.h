#pragma once

#include <algorithm>
#include <cstdlib>
#include <stack>
#include <string>
#include <vector>

#include "audiosource.h"
#include "constants.h"
#include "controller.h"
#include "object.h"
#include "path.h"
#include "utils.h"

struct Machine
{
    Machine(const Path* path);
    ~Machine();

    void run();

    void processAudioSources(double* buffer, const unsigned int bufferLength);

private:
    unsigned int readUnsignedInt(const void* buffer) const;
    int readInt(const void* buffer) const;
    double readDouble(const void* buffer) const;

    ValueObject* popStack();

    void execute(unsigned int address, const double startTime);

    Utils* utils;

    unsigned char* program;

    unsigned int programLength;

    ValueObject** resources;

    std::stack<ValueObject*> stack;

    Variable** variables;

    std::vector<AudioSource*> audioSources;

};
