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
    std::optional<bool> info;
    std::optional<double> time;
    std::optional<double> fastForward;
    std::optional<Path*> exportPath;
    std::optional<unsigned int> channels;
    std::optional<unsigned int> sampleRate;
    std::optional<unsigned int> bufferLength;
    std::optional<unsigned int> seed;
};

struct FlagParser
{
    FlagParser(char** flags, const unsigned int length);

    ProgramOptions getOptions();

private:
    std::string nextOption(const std::string flag);

    unsigned int nextInt(const std::string flag);

    double nextDouble(const std::string flag);

    std::queue<std::string> flags;

};
