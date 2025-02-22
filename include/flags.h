#pragma once

#include <exception>
#include <queue>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "path.h"
#include "utils.h"

struct ProgramOptions
{
    std::optional<double> time;
    std::optional<Path*> exportPath;
    std::optional<bool> mono;
    std::optional<unsigned int> seed;
};

struct FlagParser
{
    FlagParser(char** flags, const unsigned int length);

    ProgramOptions getOptions();

private:
    std::string nextOption(const std::string previous);

    std::queue<std::string> flags;

};
