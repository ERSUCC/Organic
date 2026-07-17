#include "../include/flags.h"
#include "../include/organic.h"
#include "../include/path.h"
#include "../include/utils.h"

int main(int argc, char** argv)
{
    try
    {
        if (argc < 2)
        {
            throw OrganicArgumentException("Not enough arguments specified.");
        }

        const Path path = Path::relative(Path::formatPath(argv[1]));

        if (!path.exists())
        {
            throw OrganicArgumentException("Specified program file does not exist.");
        }

        if (!path.isFile())
        {
            throw OrganicArgumentException("Specified program is not a file.");
        }

        Organic* organic = new Organic(path, FlagParser::parseFlags(argv + 2, argc - 2));

        organic->start();

        delete organic;

        return 0;
    }

    catch (const OrganicException& e)
    {
        Utils::printError(e.what());

        return 1;
    }
}
