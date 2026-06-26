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

    for (const Path* path : testPath("parser/success")->children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectSuccess(info);
        }
    }

    beginSuite("Parser errors");

    for (const Path* path : testPath("parser/errors")->children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectError(info);
        }
    }
}

TestParser::TestParser(TestTracker* tracker) :
    Test(tracker) {}

void TestParser::expectSuccess(const OTest* info)
{
    beginTest(info->getValue("warn")->asBoolean()->value);

    try
    {
        const SourceProvider* source = new SourceProvider(info->getSource());

        delete Parser::Parser::parseSource(source);

        delete source;
    }

    catch (const OrganicException& e)
    {
        fail("Source was not parsed successfully.");
    }

    endTest(info->getValue("name")->asString()->str);
}

void TestParser::expectError(const OTest* info)
{
    beginTest(info->getValue("warn")->asBoolean()->value);

    try
    {
        const SourceProvider* source = new SourceProvider(info->getSource());

        delete Parser::Parser::parseSource(source);

        delete source;

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

    endTest(info->getValue("name")->asString()->str);
}
