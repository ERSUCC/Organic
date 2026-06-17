#include "../include/test_resolver.h"

TestResolver::TestResolver(TestTracker* tracker) :
    Test(tracker) {}

void TestResolver::test()
{
    beginSuite("Type inference");

    for (const Path* path : sourcePath("type-resolver/inference")->children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectSuccess(info);
        }
    }

    beginSuite("Type resolver errors");

    for (const Path* path : sourcePath("type-resolver/errors")->children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectError(info);
        }
    }
}

void TestResolver::expectSuccess(const OTest* info)
{
    beginTest(info);

    try
    {
        Parser::Parser::parseSource(info->getSource())->resolveTypes(new Parser::TypeResolver());
    }

    catch (const OrganicException& e)
    {
        fail("Expected success, received an error.");
    }

    endTest();
}

void TestResolver::expectError(const OTest* info)
{
    beginTest(info);

    try
    {
        Parser::Parser::parseSource(info->getSource())->resolveTypes(new Parser::TypeResolver());

        fail("Type resolver did not throw any errors.");
    }

    catch (const OrganicParseException& e)
    {
        assert("Type resolver throws expected error", matchParseError(info, e));
    }

    catch (const OrganicException& e)
    {
        fail("Parser did not throw the expected error.");
    }

    endTest();
}
