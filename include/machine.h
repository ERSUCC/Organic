#pragma once

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "audiosource.h"
#include "constants.h"
#include "controller.h"
#include "event.h"
#include "object.h"
#include "path.h"
#include "utils.h"

struct Machine
{
    Machine(const Path* path);
    ~Machine();

    void run();

    void updateEvents();
    void processAudioSources(double* buffer, const unsigned int bufferLength);

private:
    unsigned int readUnsignedInt(const unsigned int address) const;
    int readInt(const unsigned int address) const;
    double readDouble(const unsigned int address) const;

    ValueObject* popStack();

    void execute(unsigned int address, const double startTime);

    Utils* utils;

    std::vector<unsigned char> program;

    unsigned int start;

    std::vector<ValueObject*> resources;

    std::stack<ValueObject*> stack;

    Variable** variables;

    std::vector<AudioSource*> audioSources;
    std::vector<Event*> events;

};
