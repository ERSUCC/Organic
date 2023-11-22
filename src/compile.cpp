#include "../include/compile.h"

CompilerResult Compiler::compile(char* path)
{
    Parser* parser = new Parser(path);

    Program* program = parser->parse();

    ProgramVisitor* visitor = new ProgramVisitor();

    visitor->visit(program);

    return { visitor->sources, visitor->eventQueue };
}