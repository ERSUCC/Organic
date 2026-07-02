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
    beginTest(info->getValue("warn")->asBoolean());

    const NamedSourceProvider* source = new NamedSourceProvider(info->path(), info->getSource());

    try
    {
        delete Parser::Parser::parseSource(source);
    }

    catch (const OrganicException& e)
    {
        fail("Source was not parsed successfully.");
    }

    delete source;

    endTest(info->getValue("name")->asString());
}

void TestParser::expectError(const OTest* info)
{
    beginTest(info->getValue("warn")->asBoolean());

    const NamedSourceProvider* source = new NamedSourceProvider(info->path(), info->getSource());

    try
    {
        delete Parser::Parser::parseSource(source);

        fail("Parser did not throw any errors.");
    }

    catch (const OrganicParseException& e)
    {
        assert("Parser throws expected error", matchParseError(info, e));
    }

    catch (const OrganicException& e)
    {
        fail("Parser did not throw the expected error.");
    }

    delete source;

    endTest(info->getValue("name")->asString());
}
