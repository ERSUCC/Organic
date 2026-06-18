#include "../include/test_parser.h"

TestParser::TestParser(TestTracker* tracker) :
    Test(tracker) {}

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

void TestParser::expectSuccess(const OTest* info)
{
    beginTest();

    try
    {
        Parser::Parser::parseSource(info->getSource());
    }

    catch (const OrganicException& e)
    {
        fail("Source was not parsed successfully.");
    }

    endTest(info->getValue("name")->asString()->str);
}

void TestParser::expectError(const OTest* info)
{
    beginTest();

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

    endTest(info->getValue("name")->asString()->str);
}
