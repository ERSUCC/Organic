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

    for (const Path& path : sourcePath("examples").children())
    {
        expectSuccess(path);
    }
}

TestExamples::TestExamples(TestTracker* tracker) :
    Test(tracker) {}

void TestExamples::expectSuccess(const Path& path)
{
    beginTest(false);

    const FileProvider* source = FileProvider::create(path);

    if (!source)
    {
        throw OrganicFileException("Could not read \"" + path.string() + "\".");
    }

    const Parser::Program* program = Parser::Parser::parseSource(source);

    const Parser::TypeResolver* resolver = new Parser::TypeResolver();

    TokenTransformer* transformer = new TokenTransformer(path);

    try
    {
        program->resolveTypes(resolver);

        delete program->transform(transformer);
    }

    catch (const OrganicException& e)
    {
        fail("Expected success, received an error.");
    }

    delete transformer;
    delete resolver;
    delete program;
    delete source;

    endTest(path.stem());
}
