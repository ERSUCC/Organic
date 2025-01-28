#pragma once

#include <filesystem>
#include <string>

struct Path
{
    struct Hash
    {
        std::size_t operator()(const Path* path) const
        {
            return std::hash<std::string>()(path->string());
        }
    };

    struct Equals
    {
        bool operator()(const Path* a, const Path* b) const
        {
            return a->string() == b->string();
        }
    };

    static Path* relative(const std::filesystem::path& path, const std::filesystem::path& base);
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
