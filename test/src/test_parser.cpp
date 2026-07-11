#include "../include/test_parser.h"

void TestParser::run(TestTracker* tracker)
{
    TestParser* test = new TestParser(tracker);

    test->test();

    delete test;
}

void TestParser::test()
{
    beginSuite("Parser success");

    for (const Path& path : testPath("parser/success").children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectSuccess(info);

            delete info;
        }
    }

    beginSuite("Parser errors");

    for (const Path& path : testPath("parser/errors").children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectError(info);

            delete info;
        }
    }
}

TestParser::TestParser(TestTracker* tracker) :
    Test(tracker) {}

void TestParser::expectSuccess(const OTest* info)
{
    beginTest(info);

    const NamedSourceProvider* source = new NamedSourceProvider(info->path(), info->getSource());

    try
    {
        delete Parser::Parser::parseSource(source);
    }

    catch (const OrganicException& e)
    {
        failWithError(e);
    }

    delete source;

    endTest();
}

void TestParser::expectError(const OTest* info)
{
    beginTest(info);

    const NamedSourceProvider* source = new NamedSourceProvider(info->path(), info->getSource());

    try
    {
        delete Parser::Parser::parseSource(source);

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

    delete source;

    endTest();
}
