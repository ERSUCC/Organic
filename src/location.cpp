#include "../include/location.h"

SourceLocation::SourceLocation(const Path* path, const unsigned int line, const unsigned int character) :
    path(path), line(line), character(character) {}

bool SourceLocation::operator==(const SourceLocation& other) const
{
    return path == other.path && line == other.line && character == other.character;
}

bool SourceLocation::operator!=(const SourceLocation& other) const
{
    return !(*this == other);
}
