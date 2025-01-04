#include "../include/location.h"

SourceLocation::SourceLocation(const Path* path, const unsigned int line, const unsigned int character, const unsigned int start, const unsigned int end) :
    path(path), line(line), character(character), start(start), end(end) {}
