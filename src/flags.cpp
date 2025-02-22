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

    Utils* utils = Utils::get();

    options.time = utils->infinity;

    while (!flags.empty())
    {
        std::string flag = nextOption("");

        if (setFlags.count(flag))
        {
            throw OrganicArgumentException("The option \"time\" was already set.");
        }

        if (flag == "--time")
        {
            std::string next = nextOption(flag);

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
            Path* path = Path::relative(nextOption(flag));

            if (path->parent()->exists())
            {
                throw OrganicArgumentException("Specified output file is in a non-existent directory.");
            }

            options.exportPath = path;
        }

        else if (flag == "--mono")
        {
            options.mono = true;
        }

        else
        {
            throw OrganicArgumentException("Unknown option \"" + flag + "\".");
        }
    }

    if (options.exportPath && options.time == utils->infinity)
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
