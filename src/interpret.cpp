#include "../include/interpret.h"

InterpreterResult Interpreter::interpret(const char* path, std::vector<const char*> flags)
{
    InterpreterOptions options;

    for (int i = 0; i < flags.size(); i++)
    {
        if (!strncmp(flags[i], "-t", 2) || !strncmp(flags[i], "--test", 6))
        {
            options.test = true;
            options.setTest = true;
        }

        else if (!strncmp(flags[i], "-i", 2) || !strncmp(flags[i], "--time", 6))
        {
            checkNextOption(flags, &i);

            options.time = strtod(flags[i], nullptr);
            options.setTime = true;
        }

        else if (!strncmp(flags[i], "-f", 2) || !strncmp(flags[i], "--file", 6))
        {
            checkNextOption(flags, &i);

            options.testFile = flags[i];
            options.setTestFile = true;
        }

        else
        {
            Utils::error("Unknown option '" + std::string(flags[i]) + "'.");
        }
    }

    if (!options.setTest && options.setTime)
    {
        Utils::error("Cannot set time option in normal playback mode.");
    }

    if (options.setTest && !options.setTestFile)
    {
        Utils::error("Test file must be specified in test mode.");
    }

    Parser* parser = new Parser(path);

    Program* program = parser->parse();

    ProgramVisitor* visitor = new ProgramVisitor();

    visitor->visit(program);

    if (visitor->sources.size() == 0)
    {
        Utils::error("Invalid program, no audio sources detected.");
    }

    return { visitor->sources, visitor->eventQueue, options };
}

void Interpreter::checkNextOption(std::vector<const char*>& flags, int* pos)
{
    if (++*pos >= flags.size())
    {
        Utils::error("Value must be provided for option '" + std::string(flags[*pos - 1]) + "'.");
    }
}
