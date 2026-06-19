#pragma once

#include <iterator>
#include <set>
#include <stddef.h>
#include <string>

#include "path.h"

struct SourceProvider
{
    SourceProvider(const std::string& source);

    virtual std::string description() const;

    virtual const Path* path() const;

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

    inline size_t line(const size_t offset) const
    {
        return std::distance(lineOffsets.begin(), lineOffsets.upper_bound(offset));
    }

    inline size_t character(const size_t offset) const
    {
        return offset - *(--lineOffsets.upper_bound(offset)) + 1;
    }

private:
    const std::string source;

    std::set<size_t> lineOffsets;

};

struct FileProvider : public SourceProvider
{
    static FileProvider* create(const Path* file);

    std::string description() const override;

    const Path* path() const override;

private:
    FileProvider(const Path* file, const std::string& source);

    const Path* file;

};
