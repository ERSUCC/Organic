#include "../include/interpret.h"

InterpreterResult Interpreter::interpret(const std::string path, const std::vector<const std::string>& flags)
{
    InterpreterOptions options;

    for (int i = 0; i < flags.size(); i++)
    {
        if (flags[i] == "-t" || flags[i] == "--test")
        {
            options.test = true;
            options.setTest = true;
        }

        else if (flags[i] == "-i" || flags[i] == "--time")
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

        else if (flags[i] == "-f" || flags[i] == "--file")
        {
            checkNextOption(flags, &i);

            options.testFile = flags[i];
            options.setTestFile = true;
        }

        else
        {
            Utils::argumentError("Unknown option \"" + flags[i] + "\".");
        }
    }

    if (!options.setTest && options.setTime)
    {
        Utils::argumentError("Cannot set time option in normal playback mode.");
    }

    if (options.setTest && !options.setTestFile)
    {
        Utils::argumentError("Test file must be specified in test mode.");
    }

    ProgramVisitor* visitor = new ProgramVisitor(path);

    visitor->visit((new Parser(path))->parse());

    if (visitor->sources.size() == 0)
    {
        Utils::parseError("Invalid program, no audio sources detected.", path, 0, 0);
    }

    return { visitor->sources, visitor->eventQueue, options };
}

void Interpreter::checkNextOption(const std::vector<const std::string>& flags, int* pos)
{
    if (++*pos >= flags.size())
    {
        Utils::argumentError("Value must be provided for option \"" + flags[*pos - 1] + "\".");
    }
}
