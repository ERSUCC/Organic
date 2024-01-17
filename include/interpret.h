#pragma once

#include <string>
#include <vector>
#include <queue>

#include "parse.h"
#include "token.h"

struct InterpreterOptions
{
    double time;
    std::string exportPath;
    bool mono;

    bool setTime = false;
    bool setExport = false;
    bool setMono = false;
};

struct Interpreter
{
    Interpreter(const std::string path, const std::vector<std::string> flags);

    void interpret();

    std::vector<AudioSource*> sources;
    EventQueue* eventQueue;
    InterpreterOptions options;

private:
    std::string nextOption();

    const std::string path;

    std::queue<std::string> flags;

};
