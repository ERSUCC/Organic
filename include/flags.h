#pragma once

#include <queue>
#include <string>
#include <vector>

#include "bytecode.h"
#include "parse.h"
#include "utils.h"

struct ProgramOptions
{
    double time = 0;
    std::string exportPath;
    bool mono = false;

    bool setTime = false;
    bool setExport = false;
    bool setMono = false;

    std::string bytecodePath;
};

struct FlagParser
{
    FlagParser(const std::string path, const std::vector<std::string>& flags);

    ProgramOptions getOptions();

private:
    std::string nextOption(const std::string previous);

    const std::string path;

    std::queue<std::string> flags;

};
