#include <string>
#include <vector>

#include "../include/organic.h"
#include "../include/path.h"
#include "../include/utils.h"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        Utils::argumentError("Not enough arguments specified.");
    }

    const Path* path = Path::relative(argv[1]);

    if (!path->exists())
    {
        Utils::argumentError("Specified program file does not exist.");
    }

    if (!path->isFile())
    {
        Utils::argumentError("Specified program is not a file.");
    }

    std::vector<std::string> flags;

    for (int i = 2; i < argc; i++)
    {
        flags.push_back(argv[i]);
    }

    (new Organic(path, flags))->start();

    return 0;
}
