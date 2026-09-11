#include "../include/test_examples.h"

void TestExamples::run(TestTracker* tracker)
{
    TestExamples* test = new TestExamples(tracker);

    test->test();

    delete test;
}

void TestExamples::test()
{
    beginSuite("Run examples");

    for (const Path& path : sourcePath("examples").childrenRecursive())
    {
        expectSuccess(path);
    }
}

TestExamples::TestExamples(TestTracker* tracker) :
    Test(tracker), utils(Utils::get()) {}

void TestExamples::expectSuccess(const Path& path)
{
    beginTest(path.stem(), true);

    const FileProvider* source = FileProvider::create(path);

    if (!source)
    {
        fail("Could not read \"" + path.string() + "\".");

        endTest();

        return;
    }

    const Parser::Program* program = nullptr;

    TokenTransformer* transformer = new TokenTransformer(path);

    try
    {
        program = Parser::Parser::parseSource(source);

        program->resolveTypes();

        Engine::Program* engProgram = program->transform(transformer);

        utils->time = 0;

        const size_t steps = utils->sampleRate * 10;

        double* samples = (double*)malloc(sizeof(double) * steps * utils->channels);

        engProgram->start(0);

        for (size_t i = 0; i < steps; i++)
        {
            utils->time = i * utils->timeStep;

            engProgram->processAudioSources(samples + i * utils->channels);
        }

        free(samples);

        delete engProgram;
    }

    catch (const OrganicException& e)
    {
        failWithError(e);
    }

    delete transformer;
    delete program;
    delete source;

    endTest();
}
