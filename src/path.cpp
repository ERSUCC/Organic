#include "../include/path.h"

Path* Path::relative(const std::filesystem::path& path, const std::filesystem::path& base)
{
    return new Path(base / path);
}

Path* Path::relative(const std::filesystem::path& path)
{
    return new Path(path);
}

Path* Path::beside(const std::filesystem::path& path, const Path* other)
{
    if (path.is_absolute())
    {
        return new Path(path);
    }

    return new Path(other->path.parent_path() / path);
}

bool Path::exists() const
{
    return std::filesystem::exists(path);
}

bool Path::isFile() const
{
    return std::filesystem::is_regular_file(path);
}

const Path* Path::parent() const
{
    return new Path(path.parent_path());
}

std::string Path::string() const
{
    return path.string();
}

std::string Path::stem() const
{
    return path.stem().string();
}

Path::Path(const std::filesystem::path& path) :
    path(std::filesystem::weakly_canonical(path)) {}
