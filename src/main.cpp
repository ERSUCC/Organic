#include <filesystem>
#include <string>
#include <vector>

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

    if (!std::filesystem::is_regular_file(argv[1]))
    {
        Utils::argumentError("Specified program is not a file.");
    }

    std::vector<std::string> flags;

    for (int i = 2; i < argc; i++)
    {
        flags.push_back(argv[i]);
    }

    (new Organic(std::filesystem::canonical(argv[1]), flags))->start();

    return 0;
}
