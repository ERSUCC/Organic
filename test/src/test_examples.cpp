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
    beginTest(true);

    const FileProvider* source = FileProvider::create(path);

    if (!source)
    {
        fail("Could not read \"" + path.string() + "\".");

        endTest(path.stem());

        return;
    }

    const Parser::Program* program = nullptr;
    const Parser::TypeResolver* resolver = new Parser::TypeResolver();

    TokenTransformer* transformer = new TokenTransformer(path);

    try
    {
        program = Parser::Parser::parseSource(source);

        program->resolveTypes(resolver);

        delete program->transform(transformer);
    }

    catch (const OrganicException& e)
    {
        failWithError(e);
    }

    delete transformer;
    delete resolver;
    delete program;
    delete source;

    endTest(path.stem());
}
