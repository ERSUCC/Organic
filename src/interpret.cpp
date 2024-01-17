#include "../include/interpret.h"

Interpreter::Interpreter(const std::string path, const std::vector<std::string> flags) : path(path)
{
    for (std::string flag : flags)
    {
        this->flags.push(flag);
    }
}

void Interpreter::interpret()
{
    while (!flags.empty())
    {
        std::string next = nextOption();

        if (next == "--time")
        {
            std::string next = nextOption();

            size_t end;

            options.time = std::stod(next, &end);

            if (end < next.size())
            {
                Utils::argumentError("Expected number, received \"" + next + "\".");
            }

            options.setTime = true;
        }

        else if (next == "--export")
        {
            options.exportPath = nextOption();
            options.setExport = true;
        }

        else if (next == "--mono")
        {
            options.mono = true;
            options.setMono = true;
        }

        else
        {
            Utils::argumentError("Unknown option \"" + next + "\".");
        }
    }

    if (options.setExport && !options.setTime)
    {
        Utils::argumentError("Cannot export without a time limit.");
    }

    ProgramVisitor* visitor = new ProgramVisitor(path);

    visitor->visit((new Parser(path))->parse());

    sources = visitor->sources;
    eventQueue = visitor->eventQueue;
}

std::string Interpreter::nextOption()
{
    if (flags.empty())
    {
        Utils::argumentError("Value must be provided for option \"" + flags.front() + "\".");
    }

    std::string flag = flags.front();

    flags.pop();

    return flag;
}
