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
                Utils::argumentError("The option \"time\" was already set.");
            }

            std::string next = nextOption(flag);

            size_t end;

            try
            {
                options.time = std::stod(next, &end);
            }

            catch (const std::invalid_argument& error)
            {
                Utils::argumentError("Expected number, received \"" + next + "\".");
            }

            if (end < next.size())
            {
                Utils::argumentError("Expected number, received \"" + next + "\".");
            }

            options.setTime = true;
        }

        else if (flag == "--export")
        {
            if (options.setExport)
            {
                Utils::argumentError("The option \"export\" was already set.");
            }

            Path* path = Path::relative(nextOption(flag));

            if (path->parent()->exists())
            {
                Utils::argumentError("Specified output file is in a non-existent directory.");
            }

            options.exportPath = path;
            options.setExport = true;
        }

        else if (flag == "--mono")
        {
            if (options.setMono)
            {
                Utils::argumentError("The option \"mono\" was already set.");
            }

            options.mono = true;
            options.setMono = true;
        }

        else
        {
            Utils::argumentError("Unknown option \"" + flag + "\".");
        }
    }

    if (options.setExport && !options.setTime)
    {
        Utils::argumentError("Cannot export without a time limit.");
    }

    return options;
}

std::string FlagParser::nextOption(const std::string previous)
{
    if (flags.empty())
    {
        Utils::argumentError("Value must be provided for option \"" + previous + "\".");
    }

    std::string flag = flags.front();

    flags.pop();

    return flag;
}
