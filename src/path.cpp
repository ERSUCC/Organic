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

bool Path::isDirectory() const
{
    return std::filesystem::is_directory(path);
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

bool Path::readToString(std::string& dest) const
{
    std::ifstream file(string());

    if (!file.is_open())
    {
        return false;
    }

    std::getline(file, dest, std::string::traits_type::to_char_type(std::string::traits_type::eof()));

    const bool done = file.eof();

    file.close();

    return done;
}

bool Path::readToStringBinary(std::string& dest) const
{
    std::ifstream file(string(), std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    std::ostringstream stream(std::ios::binary);

    stream << file.rdbuf();

    const bool done = file.good();

    file.close();

    dest = stream.str();

    return done;
}

Path::Path(const std::filesystem::path& path) :
    path(std::filesystem::weakly_canonical(path)) {}
