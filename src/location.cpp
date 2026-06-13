#include "../include/location.h"

SourceLocation::SourceLocation(const SourceProvider* source, const size_t start, const size_t end) :
    source(source), start(start), end(end), line(source->line(start)), character(source->character(start)) {}

bool SourceLocation::operator==(const SourceLocation& other) const
{
    return source->description() == other.source->description() && start == other.start && end == other.end;
}

bool SourceLocation::operator!=(const SourceLocation& other) const
{
    return !(*this == other);
}
