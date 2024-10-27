#pragma once

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "audiosource.h"
#include "controller.h"
#include "event.h"
#include "object.h"
#include "utils.h"

struct Machine
{
    Machine(const std::string path);

    void run();

    void updateEvents();
    void processAudioSources(double* buffer, const unsigned int bufferLength);

private:
    unsigned int readInt(const unsigned int address) const;
    double readDouble(const unsigned int address) const;

    ValueObject* popStack();

    void execute(unsigned int address, const double startTime);

    Utils* utils;

    const std::string path;

    std::vector<unsigned char> program;

    std::stack<ValueObject*> stack;

    Variable** variables;

    std::vector<AudioSource*> audioSources;
    std::vector<Event*> events;

};
