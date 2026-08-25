#include <filesystem>
#include <optional>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unordered_set>
#include <vector>

std::optional<std::string> otool(const std::filesystem::path& path)
{
    FILE* proc = popen(("otool -L \"" + path.string() + "\"").c_str(), "r");

    std::string result;

    char data[1024];

    do
    {
        fread(data, sizeof(char), 1024, proc);

        result += data;
    } while (!feof(proc));

    if (!pclose(proc))
    {
        return data;
    }

    return std::nullopt;
}

bool changeDep(const std::filesystem::path& file, const std::string& oldPath, const std::string& newPath)
{
    return !system(("install_name_tool -change \"" + oldPath + "\" \"" + newPath + "\" \"" + file.string() + "\"").c_str());
}

std::vector<std::string> getLines(const std::string& str)
{
    std::vector<std::string> lines;

    size_t pos = 0;

    std::string line;

    while (pos < str.size())
    {
        if (str[pos] == '\n')
        {
            lines.push_back(line);

            line = "";
        }

        else
        {
            line += str[pos];
        }

        pos++;
    }

    return lines;
}

std::string resolvePath(const std::string& path, const std::filesystem::path& dest)
{
    if (strncmp(path.c_str(), "@rpath", 6))
    {
        return path;
    }

    return dest / path.substr(6);
}

bool recurseDeps(const std::filesystem::path& file, const std::filesystem::path& dest, std::unordered_set<std::filesystem::path>& deps)
{
    const std::optional<std::string> result = otool(file);

    if (!result)
    {
        return false;
    }

    const std::vector<std::string> lines = getLines(result.value());

    for (size_t i = 1; i < lines.size(); i++)
    {
        const std::string source = lines[i].substr(lines[i].find_first_not_of(" \t"), lines[i].find('(') - 2);
        const std::string path = resolvePath(source, dest);
        const std::string name = std::filesystem::path(path).filename().string();

        if (std::filesystem::exists(path) && strncmp(path.c_str(), "/System/Library/Frameworks", 26))
        {
            if (strncmp(source.c_str(), "@rpath", 6))
            {
                std::filesystem::copy_file(path, dest / name, std::filesystem::copy_options::skip_existing);

                if (!changeDep(file, source, "@rpath/" + name))
                {
                    return false;
                }
            }

            if (!deps.count(path))
            {
                deps.insert(path);

                if (!recurseDeps(dest / name, dest, deps))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        return 1;
    }

    std::unordered_set<std::filesystem::path> deps;

    if (!recurseDeps(argv[1], argv[2], deps))
    {
        return 1;
    }

    return 0;
}
