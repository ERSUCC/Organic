#include <vector>
#include <string>
#include <filesystem>

#include "../include/organic.h"
#include "../include/utils.h"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        Utils::argumentError("Not enough arguments specified.");
    }

    if (!std::filesystem::exists(argv[1]))
    {
        Utils::argumentError("Specified program file does not exist.");
    }

    const std::string path = std::filesystem::canonical(argv[1]).string();

    std::vector<std::string> flags;

    for (int i = 2; i < argc; i++)
    {
        flags.push_back(argv[i]);
    }

    Organic::init(path, flags);

    return 0;
}
