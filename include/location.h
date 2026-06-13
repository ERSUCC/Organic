#pragma once

#include <string>

#include "source.h"

struct SourceLocation
{
    SourceLocation(const SourceProvider* source, const size_t start, const size_t end);

    inline std::string string() const
    {
        return source->get(start, end - start);
    }

    inline bool operator==(const SourceLocation& other) const;
    inline bool operator!=(const SourceLocation& other) const;

    const SourceProvider* source;

    const size_t start;
    const size_t end;

    const size_t line;
    const size_t character;
};
