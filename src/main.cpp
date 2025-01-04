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

    (new Organic(path, argv + 2, argc - 2))->start();

    return 0;
}
