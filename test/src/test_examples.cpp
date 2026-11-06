#include "test_examples.h"

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
    Test(tracker) {}

void TestExamples::expectSuccess(const Path& path)
{
    beginTest(path.stem(), true);

    const Path tempPath = Path::relative(std::filesystem::temp_directory_path() / ("organic-" + std::to_string(time(nullptr)) + ".wav"));

    try
    {
        ProgramOptions options;

        options.programPath = path;
        options.exportPath = tempPath;
        options.time = 60000;
        options.seed = 0;

        Organic* organic = new Organic(options);

        organic->start();

        delete organic;
    }

    catch (const OrganicException& e)
    {
        failWithError(e);
    }

    std::filesystem::remove(tempPath.string());

    endTest();
}
