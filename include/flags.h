#pragma once

#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

#include "path.h"
#include "utils.h"

struct ProgramOptions
{
    double time = 0;
    Path* exportPath = nullptr;
    bool mono = false;
};

struct FlagParser
{
    FlagParser(char** flags, const unsigned int length);

    ProgramOptions getOptions();

private:
    std::string nextOption(const std::string previous);

    std::queue<std::string> flags;

    std::unordered_set<std::string> setFlags;

};
