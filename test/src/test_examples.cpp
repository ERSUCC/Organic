#include "../include/test_examples.h"

TestExamples::TestExamples(TestTracker* tracker) :
    Test(tracker) {}

void TestExamples::test()
{
    beginSuite("Compile examples");

    for (const Path* path : sourcePath("examples")->children())
    {
        expectSuccess(path);
    }
}

void TestExamples::expectSuccess(const Path* path)
{
    beginTest();

    try
    {
        Parser::Program* program = Parser::Parser::parseSource(path);

        program->resolveTypes(new Parser::TypeResolver());
        program->transform(new TokenTransformer(path));
    }

    catch (const OrganicException& e)
    {
        fail("Expected success, received an error.");
    }

    endTest(path->stem());
}
