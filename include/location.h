#pragma once

#include "path.h"

struct SourceLocation
{
    SourceLocation(const Path* path, const unsigned int line, const unsigned int character, const unsigned int start, const unsigned int end);

    const Path* path;

    const unsigned int line;
    const unsigned int character;
    const unsigned int start;
    const unsigned int end;
};
