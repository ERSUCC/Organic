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
            if (options.setTime)
            {
                throw OrganicArgumentException("The option \"time\" was already set.");
            }

            std::string next = nextOption(flag);

            size_t end;

            try
            {
                options.time = std::stod(next, &end);
            }

            catch (const std::invalid_argument& error)
            {
                throw OrganicArgumentException("Expected number, received \"" + next + "\".");
            }

            if (end < next.size())
            {
                throw OrganicArgumentException("Expected number, received \"" + next + "\".");
            }

            options.setTime = true;
        }

        else if (flag == "--export")
        {
            if (options.setExport)
            {
                throw OrganicArgumentException("The option \"export\" was already set.");
            }

            Path* path = Path::relative(nextOption(flag));

            if (path->parent()->exists())
            {
                throw OrganicArgumentException("Specified output file is in a non-existent directory.");
            }

            options.exportPath = path;
            options.setExport = true;
        }

        else if (flag == "--mono")
        {
            if (options.setMono)
            {
                throw OrganicArgumentException("The option \"mono\" was already set.");
            }

            options.mono = true;
            options.setMono = true;
        }

        else
        {
            throw OrganicArgumentException("Unknown option \"" + flag + "\".");
        }
    }

    if (options.setExport && !options.setTime)
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
