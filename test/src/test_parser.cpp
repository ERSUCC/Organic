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
        const std::string& message = info->getValue("error")->asString()->str;

        const int line = info->getValue("line")->asInteger()->value;
        const int character = info->getValue("character")->asInteger()->value;

        assert("Parser throws expected error", e.message == message && e.location.line == line && e.location.character == character);
    }

    catch (const OrganicException& e)
    {
        fail("Parser did not throw the expected error.");
    }

    endTest();
}
