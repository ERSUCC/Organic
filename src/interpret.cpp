#include "../include/interpret.h"

InterpreterResult Interpreter::interpret(char* path, std::vector<char*> flags)
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

        else
        {
            Utils::error("Unknown option '" + std::string(flags[i]) + "'.");
        }
    }

    if (!options.setTest && options.setTime)
    {
        Utils::error("Cannot set time option in normal playback mode.");
    }

    Parser* parser = new Parser(path);

    Program* program = parser->parse();

    ProgramVisitor* visitor = new ProgramVisitor();

    visitor->visit(program);

    return { options, visitor->sources, visitor->eventQueue };
}

void Interpreter::checkNextOption(std::vector<char*>& flags, int* pos)
{
    if (++*pos >= flags.size())
    {
        Utils::error("Value must be provided for option '" + std::string(flags[*pos - 1]) + "'.");
    }
}