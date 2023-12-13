#pragma once

#include <string>
#include <vector>

#include "parse.h"
#include "token.h"

struct InterpreterOptions
{
    bool test = false;
    double time = 0;
    std::string testFile;

    bool setTest = false;
    bool setTime = false;
    bool setTestFile = false;
};

struct InterpreterResult
{
    std::vector<AudioSource*> sources;
    EventQueue* eventQueue;
    InterpreterOptions options;
};

struct Interpreter
{
    static InterpreterResult interpret(const char* path, std::vector<const char*> flags);

private:
    static void checkNextOption(std::vector<const char*>& flags, int* pos);

};
