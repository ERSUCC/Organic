#pragma once

#include <exception>
#include <queue>
#include <optional>
#include <stddef.h>
#include <string>

#include "exception.h"
#include "path.h"

struct ProgramOptions
{
    std::optional<bool> info;
    std::optional<double> time;
    std::optional<double> fastForward;
    std::optional<Path*> exportPath;
    std::optional<unsigned int> channels;
    std::optional<unsigned int> sampleRate;
    std::optional<unsigned int> bufferLength;
    std::optional<size_t> seed;
};

struct FlagParser
{
    static ProgramOptions parseFlags(char** flags, const size_t length);

private:
    FlagParser(char** flags, const size_t length);

    ProgramOptions getOptions();

    std::string nextOption(const std::string flag);

    unsigned int nextInt(const std::string flag);

    size_t nextLong(const std::string flag);

    double nextDouble(const std::string flag);

    std::queue<std::string> flags;

};
