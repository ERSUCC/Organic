#include "../include/test_parser.h"

TestParser::TestParser(TestTracker* tracker) :
    Test(tracker) {}

void TestParser::test()
{
    beginSuite("Parser success");

    for (const Path* path : sourcePath("parser/success")->children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectSuccess(info);
        }
    }

    beginSuite("Parser errors");

    for (const Path* path : sourcePath("parser/errors")->children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectError(info);
        }
    }
}

void TestParser::expectSuccess(const OTest* info)
{
    beginTest(info);

    try
    {
        Parser::Parser::parseSource(info->getSource());
    }

    catch (const OrganicException& e)
    {
        fail("Source was not parsed successfully.");
    }

    endTest();
}

void TestParser::expectError(const OTest* info)
{
    beginTest(info);

    try
    {
        Parser::Parser::parseSource(info->getSource());

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

    endTest();
}
