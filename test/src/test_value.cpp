#include "../include/test_value.h"

void TestValue::run(TestTracker* tracker)
{
    TestValue* test = new TestValue(tracker);

    test->test();

    delete test;
}

void TestValue::test()
{
    beginSuite("Expression value");

    for (const Path& path : testPath("value").children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectValue(info);

            delete info;
        }
    }
}

void TestValue::expectValue(const OTest* info)
{
    beginTest(info);

    const NamedSourceProvider* source = new NamedSourceProvider(info->path(), info->getSource());

    const Parser::Program* program = nullptr;
    const Parser::TypeResolver* resolver = new Parser::TypeResolver();

    TokenTransformer* transformer = new TokenTransformer(source->path());

    try
    {
        program = Parser::Parser::parseSource(source);

        program->resolveTypes(resolver);

        for (size_t i = 0; i < program->instructions.size() - 1; i++)
        {
            delete program->instructions[i]->transform(transformer);
        }

        Engine::ValueObject* object = program->instructions.back()->transform(transformer);

        if (object)
        {
            object->start(0);

            const double expected = info->getValue("value")->asDouble();
            const double actual = object->getValue();

            if (fabs(actual - expected) > std::numeric_limits<double>::epsilon())
            {
                fail("Expected expression to evaluate to " + TestUtils::formatDouble(expected) + ", but it evaluated to " + TestUtils::formatDouble(actual) + ".");
            }

            delete object;
        }

        else
        {
            fail("Expected an expression that returns a value.");
        }
    }

    catch (const OrganicException& e)
    {
        failWithError(e);
    }

    delete transformer;
    delete resolver;
    delete program;
    delete source;

    endTest();
}

TestValue::TestValue(TestTracker* tracker) :
    Test(tracker) {}
