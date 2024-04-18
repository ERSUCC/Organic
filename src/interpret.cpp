#include "../include/interpret.h"

Interpreter::Interpreter(const std::string path, const std::vector<std::string>& flags) : path(path)
{
    for (const std::string flag : flags)
    {
        this->flags.push(flag);
    }
}

InterpreterOptions Interpreter::interpret()
{
    InterpreterOptions options;

    while (!flags.empty())
    {
        std::string flag = nextOption("");

        if (flag == "--time")
        {
            std::string next = nextOption(flag);

            size_t end;

            try
            {
                options.time = std::stod(next, &end);
            }

            catch (std::invalid_argument error)
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
            options.exportPath = nextOption(flag);
            options.setExport = true;
        }

        else if (flag == "--mono")
        {
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

    options.bytecodePath = (new Parser::BytecodeTransformer(path))->transform((new Parser::Parser(path))->parse());

    return options;
}

std::string Interpreter::nextOption(const std::string previous)
{
    if (flags.empty())
    {
        Utils::argumentError("Value must be provided for option \"" + previous + "\".");
    }

    std::string flag = flags.front();

    flags.pop();

    return flag;
}
