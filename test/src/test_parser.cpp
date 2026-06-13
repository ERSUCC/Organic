#include "../include/test_parser.h"

TestParser::TestParser(TestTracker* tracker) :
    Test(tracker) {}

void TestParser::test()
{
    beginSuite("Parser errors");

    for (const Path* path : sourcePath("parser/errors")->children())
    {
        expectError(path);
    }
}

void TestParser::expectError(const Path* path)
{
    const OTest* info = new OTest(path);

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
