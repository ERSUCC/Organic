#include "../include/interpret.h"

InterpreterResult Interpreter::interpret(const std::string path, const std::vector<std::string>& flags)
{
    InterpreterOptions options;

    for (int i = 0; i < flags.size(); i++)
    {
        if (flags[i] == "--time")
        {
            checkNextOption(flags, &i);

            char* end;

            options.time = strtod(flags[i].c_str(), &end);

            if (*end != '\0')
            {
                Utils::argumentError("Expected number, received \"" + flags[i] + "\".");
            }

            options.setTime = true;
        }

        else if (flags[i] == "--export")
        {
            checkNextOption(flags, &i);

            options.exportPath = flags[i];
            options.setExport = true;
        }

        else if (flags[i] == "--mono")
        {
            options.mono = true;
            options.setMono = true;
        }

        else
        {
            Utils::argumentError("Unknown option \"" + flags[i] + "\".");
        }
    }

    if (options.setExport && !options.setTime)
    {
        Utils::argumentError("Cannot export without a time limit.");
    }

    ProgramVisitor* visitor = new ProgramVisitor(path);

    visitor->visit((new Parser(path))->parse());

    if (visitor->sources.size() == 0)
    {
        Utils::parseError("Invalid program, no audio sources detected.", path, 0, 0);
    }

    return { visitor->sources, visitor->eventQueue, options };
}

void Interpreter::checkNextOption(const std::vector<std::string>& flags, int* pos)
{
    if (++*pos >= flags.size())
    {
        Utils::argumentError("Value must be provided for option \"" + flags[*pos - 1] + "\".");
    }
}
