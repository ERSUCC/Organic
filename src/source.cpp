#include "../include/source.h"

SourceProvider::SourceProvider(const std::string& source) :
    source(source)
{
    lineOffsets.insert(0);

    for (size_t i = 0; i < source.size(); i++)
    {
        if (source[i] == '\n')
        {
            lineOffsets.insert(i + 1);
        }
    }
}

std::string SourceProvider::description() const
{
    return "anonymous source";
}

const Path SourceProvider::path() const
{
    return Path::relative(".");
}

FileProvider* FileProvider::create(const Path& file)
{
    std::string source;

    if (file.readToString(source))
    {
        return new FileProvider(file, source);
    }

    return nullptr;
}

std::string FileProvider::description() const
{
    return "\"" + file.string() + "\"";
}

const Path FileProvider::path() const
{
    return file;
}

FileProvider::FileProvider(const Path& file, const std::string& source) :
    SourceProvider(source), file(file) {}
