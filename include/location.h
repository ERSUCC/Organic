#pragma once

#include <filesystem>
#include <string>

struct SourceLocation
{
    SourceLocation(const std::filesystem::path& path, const unsigned int line, const unsigned int character, const unsigned int start, const unsigned int end);

    const std::filesystem::path path;

    const unsigned int line;
    const unsigned int character;
    const unsigned int start;
    const unsigned int end;
};
