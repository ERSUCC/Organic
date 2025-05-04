#pragma once

#include "path.h"

struct SourceLocation
{
    SourceLocation(const Path* path, const unsigned int line, const unsigned int character);

    bool operator==(const SourceLocation& other) const;
    bool operator!=(const SourceLocation& other) const;

    const Path* path;

    const unsigned int line;
    const unsigned int character;
};
