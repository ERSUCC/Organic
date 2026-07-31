#include "../include/test_resolver.h"

void TestResolver::run(TestTracker* tracker)
{
    TestResolver* test = new TestResolver(tracker);

    test->test();

    delete test;
}

void TestResolver::test()
{
    beginSuite("Type resolver success");

    for (const Path& path : testPath("type-resolver/success").children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectSuccess(info);

            delete info;
        }
    }

    beginSuite("Type resolver errors");

    for (const Path& path : testPath("type-resolver/errors").children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectError(info);

            delete info;
        }
    }
}

TestResolver::TestResolver(TestTracker* tracker) :
    Test(tracker) {}

void TestResolver::expectSuccess(const OTest* info)
{
    beginTest(info);

    const NamedSourceProvider* source = new NamedSourceProvider(info->path(), info->getSource());

    const Parser::Program* program = nullptr;

    try
    {
        program = Parser::Parser::parseSource(source);

        program->resolveTypes();
    }

    catch (const OrganicException& e)
    {
        failWithError(e);
    }

    delete program;
    delete source;

    endTest();
}

void TestResolver::expectError(const OTest* info)
{
    beginTest(info);

    const NamedSourceProvider* source = new NamedSourceProvider(info->path(), info->getSource());

    const Parser::Program* program = nullptr;

    try
    {
        program = Parser::Parser::parseSource(source);

        program->resolveTypes();

        fail("Expected error, but no error was thrown.");
    }

    catch (const OrganicParseException& e)
    {
        expectParseError(info, e);
    }

    catch (const OrganicException& e)
    {
        failAndCompare(info, e);
    }

    delete program;
    delete source;

    endTest();
}
