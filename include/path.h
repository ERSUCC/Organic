#pragma once

#include <filesystem>
#include <fstream>
#include <iterator>
#include <set>
#include <string>
#include <vector>

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

    std::vector<const Path*> children() const;

    std::string string() const;

    std::string stem() const;

    bool readToString(std::string& dest) const;
    bool readLines(std::vector<std::string>& dest) const;

private:
    Path(const std::filesystem::path& path);

    const std::filesystem::path path;

};

struct SourceFile
{
    static SourceFile* create(const Path* path);

    inline size_t length() const
    {
        return source.size();
    }

    inline char get(const size_t offset) const
    {
        return source[offset];
    }

    inline std::string get(const size_t offset, const size_t count) const
    {
        return source.substr(offset, count);
    }

    size_t line(const size_t offset) const;
    size_t character(const size_t offset) const;

    const Path* path;

private:
    SourceFile(const Path* path);

    bool read();

    std::string source;

    std::set<size_t> lineOffsets;

};
