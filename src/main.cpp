#include <iostream>
#include <vector>
#include <string>

#include "../include/organic.h"
#include "../include/utils.h"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        Utils::error("Not enough arguments specified.");
    }

    std::vector<const char*> flags;

    for (int i = 2; i < argc; i++)
    {
        flags.push_back(argv[i]);
    }

    Organic::init(argv[1], flags);

    return 0;
}