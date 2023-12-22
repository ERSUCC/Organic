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
    static InterpreterResult interpret(const std::string path, const std::vector<const std::string>& flags);

private:
    static void checkNextOption(const std::vector<const std::string>& flags, int* pos);

};
