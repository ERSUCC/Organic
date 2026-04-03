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
                throw OrganicArgumentException("The option \"--time\" was already set.");
            }

            options.time = nextDouble(flag);

            if (options.time <= 0)
            {
                throw OrganicArgumentException("The value provided for option \"--time\" must be positive.");
            }
        }

        else if (flag == "--export")
        {
            if (options.exportPath)
            {
                throw OrganicArgumentException("The option \"--export\" was already set.");
            }

            Path* path = Path::relative(Path::formatPath(nextOption(flag)));

            if (!path->parent()->exists())
            {
                throw OrganicArgumentException("The specified output file is in a non-existent directory.");
            }

            if (path->isDirectory())
            {
                throw OrganicArgumentException("The specified output path is a directory, it must be a file.");
            }

            options.exportPath = path;
        }

        else if (flag == "--mono")
        {
            if (options.channels)
            {
                throw OrganicArgumentException("The option \"--mono\" was already set.");
            }

            options.channels = 1;
        }

        else if (flag == "--sample-rate")
        {
            if (options.sampleRate)
            {
                throw OrganicArgumentException("The option \"--sample-rate\" was already set.");
            }

            options.sampleRate = nextInt(flag);
        }

        else if (flag == "--buffer-length")
        {
            if (options.bufferLength)
            {
                throw OrganicArgumentException("The option \"--buffer-length\" was already set.");
            }

            options.sampleRate = nextInt(flag);
        }

        else if (flag == "--seed")
        {
            if (options.seed)
            {
                throw OrganicArgumentException("The option \"--seed\" was already set.");
            }

            options.seed = nextInt(flag);
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

    if (options.exportPath && options.bufferLength)
    {
        throw OrganicArgumentException("Cannot set buffer length when exporting.");
    }

    return options;
}

std::string FlagParser::nextOption(const std::string flag)
{
    if (flags.empty())
    {
        throw OrganicArgumentException("A value must be provided for option \"" + flag + "\".");
    }

    const std::string next = flags.front();

    flags.pop();

    return next;
}

unsigned int FlagParser::nextInt(const std::string flag)
{
    const std::string next = nextOption(flag);

    if (next[0] == '-')
    {
        throw OrganicArgumentException("The value provided for option \"" + flag + "\" cannot be negative.");
    }

    unsigned int result;

    size_t end = 0;

    try
    {
        result = std::stoul(next, &end);
    }

    catch (const std::out_of_range& error)
    {
        throw OrganicArgumentException("The value provided for option \"" + flag + "\" is too large.");
    }

    catch (const std::invalid_argument& error) {}

    if (end < next.size())
    {
        throw OrganicArgumentException("Expected a positive integer for option \"" + flag + "\", received \"" + next + "\".");
    }

    return result;
}

double FlagParser::nextDouble(const std::string flag)
{
    const std::string next = nextOption(flag);

    double result;

    size_t end = 0;

    try
    {
        result = std::stod(next, &end);
    }

    catch (const std::out_of_range& error)
    {
        throw OrganicArgumentException("The value provided for option \"" + flag + "\" is too large.");
    }

    catch (const std::invalid_argument& error) {}

    if (end < next.size())
    {
        throw OrganicArgumentException("Expected a number for option \"" + flag + "\", received \"" + next + "\".");
    }

    return result;
}
