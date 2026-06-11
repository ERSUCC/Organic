#include "../include/test.h"

Test::Test(TestTracker* tracker) :
    tracker(tracker) {}

const Path* Test::sourcePath(const std::string file) const
{
    return Path::relative(file, ORGANIC_TEST_DIR);
}

void Test::beginSuite(const std::string name) const
{
    TestUtils::printSuccess("[ " + name + " ]");
}

void Test::beginTest(const TestInfo* info)
{
    current = info;

    errors.clear();

    tracker->beginSection();
}

void Test::endTest()
{
    const size_t failures = tracker->endSection();

    if (failures == 0)
    {
        TestUtils::printSuccess(current->name(), 1);
    }

    else
    {
        TestUtils::printError(current->name(), 1);

        for (const std::string& error : errors)
        {
            TestUtils::printError(error, 2);
        }

        tracker->fail();
    }
}

void Test::assert(const std::string name, const bool result)
{
    if (!result)
    {
        addError("Failed assertion: " + name);
    }
}

void Test::fail(const std::string message)
{
    addError("Error: " + message);
}

void Test::addError(const std::string text)
{
    errors.push_back(text);

    tracker->fail();
}
