#pragma once

#include "path.h"

struct SourceLocation
{
    SourceLocation(const Path* path, const unsigned int line, const unsigned int character, const unsigned int start, const unsigned int end);

    bool operator==(const SourceLocation& other) const;
    bool operator!=(const SourceLocation& other) const;

    const Path* path;

    const unsigned int line;
    const unsigned int character;
    const unsigned int start;
    const unsigned int end;
};
