#include "../include/test.h"

Test::Test(TestTracker* tracker) :
    tracker(tracker) {}

const Path Test::sourcePath(const std::string file) const
{
    return Path::relative(file, ORGANIC_SOURCE_DIR);
}

const Path Test::testPath(const std::string file) const
{
    return Path::relative(file, ORGANIC_TEST_DIR);
}

void Test::beginSuite(const std::string name) const
{
    TestUtils::printSuccess("[ " + name + " ]");
}

void Test::beginTest(const OTest* info)
{
    beginTest(info->getValue("name")->asString(), info->getValue("warn")->asBoolean());
}

void Test::beginTest(const std::string& name, const bool warn)
{
    currentTest = name;

    errors.clear();

    tracker->beginSection();

    Utils::setWarnLevel(warn ? WarnLevel::Error : WarnLevel::Suppress);
}

void Test::endTest()
{
    const size_t failures = tracker->endSection();

    if (failures == 0)
    {
        TestUtils::printSuccess(currentTest, 1);
    }

    else
    {
        TestUtils::printError(currentTest, 1);

        for (const std::string& error : errors)
        {
            TestUtils::printError(error, 2);
        }

        tracker->fail();
    }
}

void Test::fail(const std::string message)
{
    addError(message);
}

void Test::failWithError(const OrganicException& error)
{
    addError("Expected success, but received error:\n  " + error.message);
}

void Test::failAndCompare(const OTest* info, const OrganicException& error)
{
    addError("Expected error:\n  " + info->getValue("error")->asString() + "\nBut received:\n  " + error.message);
}

bool Test::matchParseError(const OTest* info, const OrganicParseException& error) const
{
    const std::string& message = info->getValue("error")->asString();

    const int line = info->getValue("line")->asInteger();
    const int character = info->getValue("character")->asInteger();

    return error.message == message && error.location.line == line && error.location.character == character;
}

void Test::addError(const std::string text)
{
    errors.push_back(text);

    tracker->fail();
}
