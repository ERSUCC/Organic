#include <vector>
#include <string>

#include "../include/organic.h"
#include "../include/utils.h"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        Utils::argumentError("Not enough arguments specified.");
    }

    std::vector<const std::string> flags;

    for (int i = 2; i < argc; i++)
    {
        flags.push_back(argv[i]);
    }

    Organic::init(argv[1], flags);

    return 0;
}
