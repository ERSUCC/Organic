#pragma once

#include <filesystem>
#include <string>

struct Path
{
    static Path* relative(const std::filesystem::path& path);
    static Path* beside(const std::filesystem::path&, const Path* other);

    bool exists() const;
    bool isFile() const;

    const Path* parent() const;

    std::string string() const;

    std::string stem() const;

private:
    Path(const std::filesystem::path& path);

    const std::filesystem::path path;

};
