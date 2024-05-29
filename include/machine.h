#pragma once

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "audiosource.h"
#include "controller.h"
#include "event.h"
#include "object.h"
#include "utils.h"

struct Machine
{
    Machine(const std::string path);

    void execute(unsigned int address);

    void processAudioSources(double* buffer, const unsigned int bufferLength);
    void performEvents();

private:
    unsigned int readInt(const unsigned int address) const;
    double readDouble(const unsigned int address) const;

    Object* popStack();
    template <typename T> T* popStackAs();
    template <typename T> List<T>* popStackAsList();

    Utils* utils;

    const std::string path;

    std::vector<unsigned char> program;

    std::stack<Object*> stack;

    std::unordered_map<unsigned char, Variable*> variables;

    std::stack<std::unordered_map<unsigned char, Object*>> inputs;

    std::vector<AudioSource*> audioSources;
    std::vector<Event*> events;

};
