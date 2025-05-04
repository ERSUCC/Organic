#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
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

    static std::filesystem::path formatPath(const std::string path);

    bool exists() const;
    bool isFile() const;
    bool isDirectory() const;

    const Path* parent() const;

    std::string string() const;

    std::string stem() const;

    bool readToString(std::string& dest) const;
    bool readToStringBinary(std::string& dest) const;

private:
    Path(const std::filesystem::path& path);

    const std::filesystem::path path;

};
