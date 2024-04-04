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

    std::string bytecodePath;
};

struct Interpreter
{
    Interpreter(const std::string path, const std::vector<std::string> flags);

    InterpreterOptions interpret();

private:
    std::string nextOption(const std::string previous);

    const std::string path;

    std::queue<std::string> flags;

};
