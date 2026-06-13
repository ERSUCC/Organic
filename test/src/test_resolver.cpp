#include "../include/test_resolver.h"

TestResolver::TestResolver(TestTracker* tracker) :
    Test(tracker) {}

void TestResolver::test()
{
    beginSuite("Type inference");

    for (const Path* path : sourcePath("type-resolver/inference")->children())
    {
        expectSuccess(path);
    }

    beginSuite("Type resolver errors");

    for (const Path* path : sourcePath("type-resolver/errors")->children())
    {
        expectError(path);
    }
}

void TestResolver::expectSuccess(const Path* path)
{
    const OTest* info = new OTest(path);

    beginTest(info);

    try
    {
        Parser::Parser::parseSource(info->getSource())->resolveTypes(new Parser::TypeResolver(path));
    }

    catch (const OrganicException& e)
    {
        fail("Expected success, received an error.");
    }

    endTest();
}

void TestResolver::expectError(const Path* path)
{
    const OTest* info = new OTest(path);

    beginTest(info);

    try
    {
        Parser::Parser::parseSource(info->getSource())->resolveTypes(new Parser::TypeResolver(path));

        fail("Type resolver did not throw any errors.");
    }

    catch (const OrganicParseException& e)
    {
        const std::string& message = info->getValue("error")->asString()->str;

        const int line = info->getValue("line")->asInteger()->value;
        const int character = info->getValue("character")->asInteger()->value;

        assert("Type resolver throws expected error", e.message == message && e.location.line == line && e.location.character == character);
    }

    catch (const OrganicException& e)
    {
        fail("Parser did not throw the expected error.");
    }

    endTest();
}
