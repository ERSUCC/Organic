#pragma once

#include <string>
#include <vector>

#include "parse.h"
#include "token.h"

struct InterpreterOptions
{
    double time = 0;

    bool setTime = false;
};

struct InterpreterResult
{
    std::vector<AudioSource*> sources;
    EventQueue* eventQueue;
    InterpreterOptions options;
};

struct Interpreter
{
    static InterpreterResult interpret(const std::string path, const std::vector<std::string>& flags);

private:
    static void checkNextOption(const std::vector<std::string>& flags, int* pos);

};
