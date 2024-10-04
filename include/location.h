#pragma once

#include <string>

struct SourceLocation
{
    SourceLocation(const std::string path, const unsigned int line, const unsigned int character, const unsigned int start, const unsigned int end);

    const std::string path;

    const unsigned int line;
    const unsigned int character;
    const unsigned int start;
    const unsigned int end;
};
