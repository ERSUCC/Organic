#pragma once

#include <queue>
#include <string>
#include <vector>

#include "path.h"
#include "utils.h"

struct ProgramOptions
{
    double time = 0;
    Path* exportPath;
    bool mono = false;

    bool setTime = false;
    bool setExport = false;
    bool setMono = false;
};

struct FlagParser
{
    FlagParser(const std::vector<std::string>& flags);

    ProgramOptions getOptions();

private:
    std::string nextOption(const std::string previous);

    std::queue<std::string> flags;

};
