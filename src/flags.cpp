#include "../include/flags.h"

ProgramOptions FlagParser::parseFlags(char** flags, const size_t length)
{
    FlagParser* parser = new FlagParser(flags, length);

    ProgramOptions options = parser->getOptions();

    delete parser;

    return options;
}

FlagParser::FlagParser(char** flags, const size_t length)
{
    for (size_t i = 0; i < length; i++)
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

        if (flag == "--info")
        {
            if (options.info)
            {
                throw OrganicArgumentException("The option \"--info\" was already set.");
            }

            options.info = true;
        }

        else if (flag == "--time")
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

        else if (flag == "--fast-forward")
        {
            if (options.fastForward)
            {
                throw OrganicArgumentException("The option \"--fast-forward\" was already set.");
            }

            options.fastForward = nextDouble(flag);

            if (options.fastForward < 0)
            {
                throw OrganicArgumentException("The value provided for option \"--fast-forward\" cannot be negative.");
            }
        }

        else if (flag == "--export")
        {
            if (options.exportPath)
            {
                throw OrganicArgumentException("The option \"--export\" was already set.");
            }

            const Path path = Path::relative(Path::formatPath(nextOption(flag)));

            if (!path.parent().exists())
            {
                throw OrganicArgumentException("The specified output file is in a non-existent directory.");
            }

            if (path.isDirectory())
            {
                throw OrganicArgumentException("The specified output path is a directory, it must be a file.");
            }

            options.exportPath.emplace(path);
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

            options.bufferLength = nextInt(flag);
        }

        else if (flag == "--seed")
        {
            if (options.seed)
            {
                throw OrganicArgumentException("The option \"--seed\" was already set.");
            }

            options.seed = nextLong(flag);
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

    if (options.exportPath && options.fastForward)
    {
        throw OrganicArgumentException("Cannot fast forward when exporting.");
    }

    if (options.exportPath && options.bufferLength)
    {
        throw OrganicArgumentException("Cannot set buffer length when exporting.");
    }

    return options;
}

std::string FlagParser::nextOption(const std::string& flag)
{
    if (flags.empty())
    {
        throw OrganicArgumentException("A value must be provided for option \"" + flag + "\".");
    }

    const std::string next = flags.front();

    flags.pop();

    return next;
}

unsigned int FlagParser::nextInt(const std::string& flag)
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

size_t FlagParser::nextLong(const std::string& flag)
{
    const std::string next = nextOption(flag);

    if (next[0] == '-')
    {
        throw OrganicArgumentException("The value provided for option \"" + flag + "\" cannot be negative.");
    }

    size_t result;
    size_t end = 0;

    try
    {
        result = std::stoull(next, &end);
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

double FlagParser::nextDouble(const std::string& flag)
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
