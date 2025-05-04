#include "../include/flags.h"

FlagParser::FlagParser(char** flags, const unsigned int length)
{
    for (unsigned int i = 0; i < length; i++)
    {
        this->flags.push(flags[i]);
    }
}

ProgramOptions FlagParser::getOptions()
{
    ProgramOptions options;

    while (!flags.empty())
    {
        std::string flag = nextOption("");

        if (flag == "--time")
        {
            if (options.time)
            {
                throw OrganicArgumentException("The option \"time\" was already set.");
            }

            const std::string next = nextOption(flag);

            size_t end = 0;

            try
            {
                options.time = std::stod(next, &end);
            }

            catch (const std::invalid_argument& error) {}

            if (end < next.size())
            {
                throw OrganicArgumentException("Expected number, received \"" + next + "\".");
            }

            if (options.time <= 0)
            {
                throw OrganicArgumentException("Time limit must be greater than zero.");
            }
        }

        else if (flag == "--export")
        {
            if (options.exportPath)
            {
                throw OrganicArgumentException("The option \"export\" was already set.");
            }

            Path* path = Path::relative(Path::formatPath(nextOption(flag)));

            if (!path->parent()->exists())
            {
                throw OrganicArgumentException("Specified output file is in a non-existent directory.");
            }

            if (path->isDirectory())
            {
                throw OrganicArgumentException("Specified output path is a directory, it must be a file.");
            }

            options.exportPath = path;
        }

        else if (flag == "--mono")
        {
            if (options.mono)
            {
                throw OrganicArgumentException("The option \"mono\" was already set.");
            }

            options.mono = true;
        }

        else if (flag == "--seed")
        {
            if (options.seed)
            {
                if (options.seed)
                {
                    throw OrganicArgumentException("The option \"seed\" was already set.");
                }
            }

            const std::string next = nextOption(flag);

            if (next[0] == '-')
            {
                throw OrganicArgumentException("Random seed must be greater than or equal to zero.");
            }

            size_t end = 0;

            try
            {
                options.seed = std::stoul(next, &end);
            }

            catch (const std::out_of_range& error)
            {
                throw OrganicArgumentException("Random seed must be in the range of a 32-bit unsigned integer.");
            }

            catch (const std::invalid_argument& error) {}

            if (end < next.size())
            {
                throw OrganicArgumentException("Expected unsigned integer, received \"" + next + "\".");
            }
        }

        else
        {
            throw OrganicArgumentException("Unknown option \"" + flag + "\".");
        }
    }

    if (options.exportPath && !options.time)
    {
        throw OrganicArgumentException("Cannot export without a time limit.");
    }

    return options;
}

std::string FlagParser::nextOption(const std::string previous)
{
    if (flags.empty())
    {
        throw OrganicArgumentException("Value must be provided for option \"" + previous + "\".");
    }

    std::string flag = flags.front();

    flags.pop();

    return flag;
}
