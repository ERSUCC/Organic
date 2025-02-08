#include "../include/test.h"

void Test::testAll()
{
    (new ParserTest())->test();
}

const Path* Test::sourcePath(const std::string file) const
{
    return Path::relative(file, ORGANIC_TEST_DIR);
}

const Parser::Program* Test::parseSource(const Path* path) const
{
    return (new Parser::Parser(path, std::unordered_set<const Path*, Path::Hash, Path::Equals> { path }))->parse();
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

void ParserTest::test()
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

bool ParserTest::semanticEquals(const Parser::Token* expected, const Parser::Token* actual) const
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

    else if (const Parser::Include* include = dynamic_cast<const Parser::Include*>(expected))
    {
        if (const Parser::Include* other = dynamic_cast<const Parser::Include*>(actual))
        {
            return semanticEquals(include->program, other->program);
        }
    }

    else if (const Parser::Define* define = dynamic_cast<const Parser::Define*>(expected))
    {
        if (const Parser::Define* other = dynamic_cast<const Parser::Define*>(actual))
        {
            if (define->name != other->name || define->inputs.size() != other->inputs.size() || define->instructions.size() != other->instructions.size())
            {
                return false;
            }

            for (unsigned int i = 0; i < define->inputs.size(); i++)
            {
                if (!semanticEquals(define->inputs[i], other->inputs[i]))
                {
                    return false;
                }
            }

            for (unsigned int i = 0; i < define->instructions.size(); i++)
            {
                if (!semanticEquals(define->instructions[i], other->instructions[i]))
                {
                    return false;
                }
            }

            return true;
        }
    }

    else if (const Parser::CallUser* call = dynamic_cast<const Parser::CallUser*>(expected))
    {
        if (const Parser::CallUser* other = dynamic_cast<const Parser::CallUser*>(actual))
        {
            if (call->arguments->name != other->arguments->name || call->topLevel != other->topLevel || call->arguments->arguments.size() != other->arguments->arguments.size())
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

            return semanticEquals(call->function, other->function);
        }
    }

    else if (const Parser::Call* call = dynamic_cast<const Parser::Call*>(expected))
    {
        if (const Parser::Call* other = dynamic_cast<const Parser::Call*>(actual))
        {
            if (call->arguments->name != other->arguments->name || call->topLevel != other->topLevel || call->arguments->arguments.size() != other->arguments->arguments.size())
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

    else if (const Parser::CallAlias* call = dynamic_cast<const Parser::CallAlias*>(expected))
    {
        if (const Parser::CallAlias* other = dynamic_cast<const Parser::CallAlias*>(actual))
        {
            return semanticEquals(call->a, other->a) && semanticEquals(call->b, other->b);
        }
    }

    else if (const Parser::Assign* assign = dynamic_cast<const Parser::Assign*>(expected))
    {
        if (const Parser::Assign* other = dynamic_cast<const Parser::Assign*>(actual))
        {
            return semanticEquals(assign->variable, other->variable) && semanticEquals(assign->value, other->value);
        }
    }

    else if (const Parser::ParenthesizedExpression* exp = dynamic_cast<const Parser::ParenthesizedExpression*>(expected))
    {
        if (const Parser::ParenthesizedExpression* other = dynamic_cast<const Parser::ParenthesizedExpression*>(actual))
        {
            return semanticEquals(exp->value, other->value);
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

    else if (const Parser::Argument* argument = dynamic_cast<const Parser::Argument*>(expected))
    {
        if (const Parser::Argument* other = dynamic_cast<const Parser::Argument*>(actual))
        {
            return argument->name == other->name && semanticEquals(argument->value, other->value);
        }
    }

    else if (dynamic_cast<const Parser::Ignore*>(expected))
    {
        return dynamic_cast<const Parser::Ignore*>(actual);
    }

    else if (const Parser::String* str = dynamic_cast<const Parser::String*>(expected))
    {
        if (const Parser::String* other = dynamic_cast<const Parser::String*>(actual))
        {
            return str->str == other->str;
        }
    }

    else if (dynamic_cast<const Parser::SequenceForwards*>(expected))
    {
        return dynamic_cast<const Parser::SequenceForwards*>(actual);
    }

    else if (dynamic_cast<const Parser::SequenceBackwards*>(expected))
    {
        return dynamic_cast<const Parser::SequenceBackwards*>(actual);
    }

    else if (dynamic_cast<const Parser::SequencePingPong*>(expected))
    {
        return dynamic_cast<const Parser::SequencePingPong*>(actual);
    }

    else if (dynamic_cast<const Parser::SequenceRandom*>(expected))
    {
        return dynamic_cast<const Parser::SequenceRandom*>(actual);
    }

    else if (dynamic_cast<const Parser::RandomStep*>(expected))
    {
        return dynamic_cast<const Parser::RandomStep*>(actual);
    }

    else if (dynamic_cast<const Parser::RandomLinear*>(expected))
    {
        return dynamic_cast<const Parser::RandomLinear*>(actual);
    }

    else if (dynamic_cast<const Parser::Pi*>(expected))
    {
        return dynamic_cast<const Parser::Pi*>(actual);
    }

    else if (dynamic_cast<const Parser::E*>(expected))
    {
        return dynamic_cast<const Parser::E*>(actual);
    }

    else if (const Parser::Value* value = dynamic_cast<const Parser::Value*>(expected))
    {
        if (const Parser::Value* other = dynamic_cast<const Parser::Value*>(actual))
        {
            return value->value == other->value;
        }
    }

    return false;
}
