#pragma once

#include <string>
#include <vector>

#include "parse.h"
#include "token.h"

struct InterpreterOptions
{
    bool test = false;
    double time = 0;

    bool setTest = false;
    bool setTime = false;
};

struct InterpreterResult
{
    InterpreterOptions options;

    std::vector<AudioSource*> sources;
    EventQueue* eventQueue;
};

struct Interpreter
{
    static InterpreterResult interpret(char* path, std::vector<char*> flags);

private:
    static void checkNextOption(std::vector<char*>& flags, int* pos);

};