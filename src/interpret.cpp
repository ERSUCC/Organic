#include "../include/interpret.h"

InterpreterResult Interpreter::interpret(char* path)
{
    Parser* parser = new Parser(path);

    Program* program = parser->parse();

    ProgramVisitor* visitor = new ProgramVisitor();

    visitor->visit(program);

    return { visitor->sources, visitor->eventQueue };
}