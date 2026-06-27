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
        }
    }

    beginSuite("Type resolver errors");

    for (const Path& path : testPath("type-resolver/errors").children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectError(info);
        }
    }
}

TestResolver::TestResolver(TestTracker* tracker) :
    Test(tracker) {}

void TestResolver::expectSuccess(const OTest* info)
{
    beginTest(info->getValue("warn")->asBoolean()->value);

    try
    {
        const SourceProvider* source = new SourceProvider(info->getSource());

        const Parser::Program* program = Parser::Parser::parseSource(source);

        program->resolveTypes(new Parser::TypeResolver());

        delete program;
        delete source;
    }

    catch (const OrganicException& e)
    {
        fail("Expected success, received an error.");
    }

    endTest(info->getValue("name")->asString()->str);
}

void TestResolver::expectError(const OTest* info)
{
    beginTest(info->getValue("warn")->asBoolean()->value);

    try
    {
        const SourceProvider* source = new SourceProvider(info->getSource());

        const Parser::Program* program = Parser::Parser::parseSource(source);

        program->resolveTypes(new Parser::TypeResolver());

        delete program;
        delete source;

        fail("Type resolver did not throw any errors.");
    }

    catch (const OrganicParseException& e)
    {
        assert("Type resolver throws expected error", matchParseError(info, e));
    }

    catch (const OrganicException& e)
    {
        fail("Parser did not throw the expected error.");
    }

    endTest(info->getValue("name")->asString()->str);
}
