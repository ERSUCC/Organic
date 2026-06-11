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

    endSuite();
}

void TestParser::expectError(const Path* path)
{
    TestErrorInfo* info = new TestErrorInfo(path);

    beginTest(info);

    try
    {
        Parser::Parser::parseSource(path);

        fail("Parser did not throw any errors.");
    }

    catch (const OrganicParseException& e)
    {
        assert("Parser throws expected error", e.location.source->path == path && info->matches(e));
    }

    catch (const OrganicException& e)
    {
        fail("Parser did not throw the expected error.");
    }

    endTest();
}
