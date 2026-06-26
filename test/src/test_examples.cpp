#include "../include/test_examples.h"

void TestExamples::run(TestTracker* tracker)
{
    TestExamples* test = new TestExamples(tracker);

    test->test();

    delete test;
}

void TestExamples::test()
{
    beginSuite("Compile examples");

    for (const Path* path : sourcePath("examples")->children())
    {
        expectSuccess(path);
    }
}

TestExamples::TestExamples(TestTracker* tracker) :
    Test(tracker) {}

void TestExamples::expectSuccess(const Path* path)
{
    beginTest(false);

    try
    {
        const FileProvider* source = FileProvider::create(path);

        if (!source)
        {
            throw OrganicFileException("Could not read \"" + path->string() + "\".");
        }

        const Parser::Program* program = Parser::Parser::parseSource(source);

        Parser::TypeResolver* resolver = new Parser::TypeResolver();

        TokenTransformer* transformer = new TokenTransformer(path);

        program->resolveTypes(resolver);
        program->transform(transformer);

        delete program;
        delete resolver;
        delete transformer;
        delete source;
    }

    catch (const OrganicException& e)
    {
        fail("Expected success, received an error.");
    }

    endTest(path->stem());
}
