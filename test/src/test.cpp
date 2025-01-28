#include "../include/test.h"

void Test::testAll()
{
    testParser();
}

void Test::testParser()
{
    beginSection("Test parser");

    beginTest("Empty file");

    {
        const Path* path = sourcePath("empty.organic");
        const Parser::Program* program = parseSource(path);

        const Parser::Program* expected = new Parser::Program(SourceLocation(path, 1, 1, 0, 0), {});

        assert("Program semantically matches expected", semanticEquals(expected, program));
    }

    endTest();

    beginTest("Empty file with comments");

    {
        const Path* path = sourcePath("empty-with-comments.organic");
        const Parser::Program* program = parseSource(path);

        const Parser::Program* expected = new Parser::Program(SourceLocation(path, 1, 1, 0, 0), {});

        assert("Program semantically matches expected", semanticEquals(expected, program));
    }

    endTest();

    beginTest("Hello world");

    {
        const Path* path = sourcePath("hello-world.organic");
        const Parser::Program* program = parseSource(path);

        const Parser::Program* expected = new Parser::Program(SourceLocation(path, 1, 1, 0, 10),
        {
            new Parser::Sine(SourceLocation(path, 1, 1, 0, 10), new Parser::ArgumentList(
            {
                new Parser::Argument(SourceLocation(path, 1, 6, 2, 5), "volume", new Parser::Value(SourceLocation(path, 1, 14, 4, 5), "1", 1)),
                new Parser::Argument(SourceLocation(path, 1, 17, 6, 9), "frequency", new Parser::Value(SourceLocation(path, 1, 28, 8, 9), "440", 440))
            }, "sine"), true)
        });

        assert("Program semantically matches expected", semanticEquals(expected, program));
    }

    endTest();

    beginTest("Single nest");

    {
        const Path* path = sourcePath("single-nest.organic");
        const Parser::Program* program = parseSource(path);

        const Parser::Program* expected = new Parser::Program(SourceLocation(path, 1, 1, 0, 23),
        {
            new Parser::Sine(SourceLocation(path, 1, 1, 0, 23), new Parser::ArgumentList(
            {
                new Parser::Argument(SourceLocation(path, 1, 6, 2, 5), "volume", new Parser::Value(SourceLocation(path, 1, 14, 4, 5), "1", 1)),
                new Parser::Argument(SourceLocation(path, 1, 17, 6, 22), "frequency", new Parser::Sweep(SourceLocation(path, 1, 28, 8, 22), new Parser::ArgumentList(
                {
                    new Parser::Argument(SourceLocation(path, 1, 34, 10, 13), "from", new Parser::Value(SourceLocation(path, 1, 40, 12, 13), "110", 110)),
                    new Parser::Argument(SourceLocation(path, 1, 45, 14, 17), "to", new Parser::Value(SourceLocation(path, 1, 49, 16, 17), "220", 220)),
                    new Parser::Argument(SourceLocation(path, 1, 54, 18, 21), "length", new Parser::Value(SourceLocation(path, 1, 62, 20, 21), "1000", 1000))
                }, "sweep"), false))
            }, "sine"), true)
        });

        assert("Program semantically matches expected", semanticEquals(expected, program));
    }

    endTest();

    beginTest("Basic list");

    {
        const Path* path = sourcePath("basic-list.organic");
        const Parser::Program* program = parseSource(path);

        const Parser::Program* expected = new Parser::Program(SourceLocation(path, 1, 1, 0, 37),
        {
            new Parser::Sine(SourceLocation(path, 1, 1, 0, 37), new Parser::ArgumentList(
            {
                new Parser::Argument(SourceLocation(path, 1, 6, 2, 5), "volume", new Parser::Value(SourceLocation(path, 1, 14, 4, 5), "1", 1)),
                new Parser::Argument(SourceLocation(path, 1, 17, 6, 36), "frequency", new Parser::Sequence(SourceLocation(path, 1, 28, 8, 36), new Parser::ArgumentList(
                {
                    new Parser::Argument(SourceLocation(path, 1, 37, 10, 35), "values", new Parser::List(SourceLocation(path, 1, 45, 12, 35),
                    {
                        new Parser::Hold(SourceLocation(path, 2, 5, 13, 23), new Parser::ArgumentList(
                        {
                            new Parser::Argument(SourceLocation(path, 2, 10, 15, 18), "value", new Parser::Value(SourceLocation(path, 2, 17, 17, 18), "110", 110)),
                            new Parser::Argument(SourceLocation(path, 2, 22, 19, 22), "length", new Parser::Value(SourceLocation(path, 2, 30, 21, 22), "1000", 1000))
                        }, "hold"), false),
                        new Parser::Hold(SourceLocation(path, 3, 5, 24, 34), new Parser::ArgumentList(
                        {
                            new Parser::Argument(SourceLocation(path, 3, 10, 26, 29), "value", new Parser::Value(SourceLocation(path, 3, 17, 28, 29), "220", 220)),
                            new Parser::Argument(SourceLocation(path, 3, 22, 30, 33), "length", new Parser::Value(SourceLocation(path, 3, 30, 32, 33), "1000", 1000))
                        }, "hold"), false)
                    }))
                }, "sequence"), false))
            }, "sine"), true)
        });

        assert("Program semantically matches expected", semanticEquals(expected, program));
    }

    endTest();

    endSection();
}

const Path* Test::sourcePath(const std::string file) const
{
    return Path::relative(file, ORGANIC_TEST_DIR);
}

const Parser::Program* Test::parseSource(const Path* path) const
{
    return (new Parser::Parser(path, std::unordered_set<const Path*, Path::Hash, Path::Equals> { path }))->parse();
}

bool Test::semanticEquals(const Parser::Token* expected, const Parser::Token* actual) const
{
    if (expected->location != actual->location)
    {
        return false;
    }

    if (const Parser::Program* program = dynamic_cast<const Parser::Program*>(expected))
    {
        if (const Parser::Program* other = dynamic_cast<const Parser::Program*>(actual))
        {
            if (program->instructions.size() != other->instructions.size())
            {
                return false;
            }

            for (unsigned int i = 0; i < program->instructions.size(); i++)
            {
                if (!semanticEquals(program->instructions[i], other->instructions[i]))
                {
                    return false;
                }
            }

            return true;
        }
    }

    else if (const Parser::Call* call = dynamic_cast<const Parser::Call*>(expected))
    {
        if (const Parser::Call* other = dynamic_cast<const Parser::Call*>(actual))
        {
            if (call->topLevel != other->topLevel)
            {
                return false;
            }

            if (call->arguments->arguments.size() != other->arguments->arguments.size())
            {
                return false;
            }

            for (unsigned int i = 0; i < call->arguments->arguments.size(); i++)
            {
                if (!semanticEquals(call->arguments->arguments[i], other->arguments->arguments[i]))
                {
                    return false;
                }
            }

            return true;
        }
    }

    else if (const Parser::Argument* argument = dynamic_cast<const Parser::Argument*>(expected))
    {
        if (const Parser::Argument* other = dynamic_cast<const Parser::Argument*>(actual))
        {
            return argument->name == other->name && semanticEquals(argument->value, other->value);
        }
    }

    else if (const Parser::List* list = dynamic_cast<const Parser::List*>(expected))
    {
        if (const Parser::List* other = dynamic_cast<const Parser::List*>(actual))
        {
            if (list->values.size() != other->values.size())
            {
                return false;
            }

            for (unsigned int i = 0; i < list->values.size(); i++)
            {
                if (!semanticEquals(list->values[i], other->values[i]))
                {
                    return false;
                }
            }

            return true;
        }
    }

    else if (const Parser::Value* value = dynamic_cast<const Parser::Value*>(expected))
    {
        if (const Parser::Value* other = dynamic_cast<const Parser::Value*>(actual))
        {
            return value->str == other->str && value->value == other->value;
        }
    }

    return false;
}

void Test::print(const std::string text)
{
    for (unsigned int i = 0; i < indents; i++)
    {
        std::cout << '\t';
    }

    std::cout << text << "\n";
}

void Test::beginSection(const std::string name)
{
    print("[ " + name + " ]");

    indents++;
}

void Test::endSection()
{
    indents--;
}

void Test::beginTest(const std::string name)
{
    print(name);

    indents++;

    failures = 0;
}

void Test::endTest()
{
    indents--;

    if (failures == 0)
    {
        print("Test passed");
    }

    else
    {
        if (failures == 1)
        {
            print("Test failed with " + std::to_string(failures) + " incorrect assertion");
        }

        else
        {
            print("Test failed with " + std::to_string(failures) + " incorrect assertions");
        }
    }
}

void Test::assert(const std::string name, const bool result)
{
    if (!result)
    {
        print("Failed assertion: \"" + name + "\"");

        failures++;
    }
}
