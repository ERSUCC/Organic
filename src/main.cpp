#include <string.h>

#include "exception.h"
#include "flags.h"
#include "organic.h"
#include "utils.h"

int main(int argc, char** argv)
{
    try
    {
        if (argc < 2)
        {
            Utils::printUsage();

            return 1;
        }

        if (!strncmp(argv[1], "--version", 10))
        {
            Utils::printVersion();

            return 0;
        }

        Organic* organic = new Organic(FlagParser::parseFlags(argv + 1, argc - 1));

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
