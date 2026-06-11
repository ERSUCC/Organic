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

    endSuite();

    beginSuite("Type resolver errors");

    for (const Path* path : sourcePath("type-resolver/errors")->children())
    {
        expectError(path);
    }

    endSuite();
}

void TestResolver::expectSuccess(const Path* path)
{
    beginTest(new TestInfo(path));

    try
    {
        Parser::Parser::parseSource(path)->resolveTypes(new Parser::TypeResolver(path));
    }

    catch (const OrganicException& e)
    {
        fail("Expected success, received an error.");
    }

    endTest();
}

void TestResolver::expectError(const Path* path)
{
    TestErrorInfo* info = new TestErrorInfo(path);

    beginTest(info);

    try
    {
        Parser::Parser::parseSource(path)->resolveTypes(new Parser::TypeResolver(path));

        fail("Type resolver did not throw any errors.");
    }

    catch (const OrganicParseException& e)
    {
        assert("Type resolver throws expected error", e.location.source->path == path && info->matches(e));
    }

    catch (const OrganicException& e)
    {
        fail("Parser did not throw the expected error.");
    }

    endTest();
}
