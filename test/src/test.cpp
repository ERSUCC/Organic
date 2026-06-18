#include "../include/test.h"

Test::Test(TestTracker* tracker) :
    tracker(tracker) {}

const Path* Test::sourcePath(const std::string file) const
{
    return Path::relative(file, ORGANIC_SOURCE_DIR);
}

const Path* Test::testPath(const std::string file) const
{
    return Path::relative(file, ORGANIC_TEST_DIR);
}

void Test::beginSuite(const std::string name) const
{
    TestUtils::printSuccess("[ " + name + " ]");
}

void Test::beginTest()
{
    errors.clear();

    tracker->beginSection();
}

void Test::endTest(const std::string name)
{
    const size_t failures = tracker->endSection();

    if (failures == 0)
    {
        TestUtils::printSuccess(name, 1);
    }

    else
    {
        TestUtils::printError(name, 1);

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

bool Test::matchParseError(const OTest* info, const OrganicParseException& error) const
{
    const std::string& message = info->getValue("error")->asString()->str;

    const int line = info->getValue("line")->asInteger()->value;
    const int character = info->getValue("character")->asInteger()->value;

    return error.message == message && error.location.line == line && error.location.character == character;
}

void Test::addError(const std::string text)
{
    errors.push_back(text);

    tracker->fail();
}
