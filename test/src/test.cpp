#include "../include/test.h"

TestInfo::TestInfo(const Path* path)
{
    std::vector<std::string> lines;

    if (!path->readLines(lines))
    {
        throw OrganicFileException("Could not open \"" + path->string() + "\".");
    }

    for (size_t i = 0; i < lines.size() && lines[i].substr(0, 2) == "//"; i++)
    {
        data.push_back({});

        char* line = lines[i].data() + 3;

        int length = lines[i].size() - 3;

        while (length > 0)
        {
            size_t j = 2;

            while (j < length && (line[j] != ' ' || line[j - 1] != '~' || line[j - 2] != ' '))
            {
                j++;
            }

            if (j < length)
            {
                line[j - 2] = '\0';
            }

            data[i].push_back(line);

            line += j + 1;
            length -= j + 1;
        }
    }
}

std::string TestInfo::name() const
{
    return data[0][0];
}

TestErrorInfo::TestErrorInfo(const Path* path) :
    TestInfo(path), line(std::stoi(data[1][0])), character(std::stoi(data[1][1])), message(data[1][2]) {}

bool TestErrorInfo::matches(const OrganicParseException& error) const
{
    return message == error.message && line == error.location.line && character == error.location.character;
}

TestTokenizerInfo::TestTokenizerInfo(const Path* path) :
    TestInfo(path) {}

const std::vector<std::string>& TestTokenizerInfo::tokens() const
{
    return data[1];
}

void TestTracker::beginSection()
{
    failures.push(0);
}

void TestTracker::fail()
{
    failures.top()++;
}

size_t TestTracker::endSection()
{
    const size_t result = failures.top();

    failures.pop();

    return result;
}

Test::Test(TestTracker* tracker) :
    tracker(tracker) {}

const Path* Test::sourcePath(const std::string file) const
{
    return Path::relative(file, ORGANIC_TEST_DIR);
}

void Test::print(const std::string text)
{
    for (size_t i = 0; i < indents; i++)
    {
        std::cout << "  - ";
    }

    std::cout << text << "\n";
}

void Test::beginSuite(const std::string name)
{
    print("[ " + name + " ]");

    indents++;

    tracker->beginSection();
}

void Test::endSuite()
{
    indents--;

    const size_t failures = tracker->endSection();

    if (failures == 0)
    {
        print("[ Suite passed ]");
    }

    else if (failures == 1)
    {
        print("[ Suite had 1 failing test ]");

        tracker->fail();
    }

    else
    {
        print("[ Suite had " + std::to_string(failures) + " failing tests ]");

        tracker->fail();
    }
}

void Test::beginTest(const TestInfo* info)
{
    print(info->name());

    indents++;

    tracker->beginSection();
}

void Test::endTest()
{
    indents--;

    const size_t failures = tracker->endSection();

    if (failures == 1)
    {
        print("Test failed with " + std::to_string(failures) + " incorrect assertion");

        tracker->fail();
    }

    else if (failures != 0)
    {
        print("Test failed with " + std::to_string(failures) + " incorrect assertions");

        tracker->fail();
    }
}

void Test::assert(const std::string name, const bool result)
{
    if (!result)
    {
        print("Failed assertion: " + name);

        tracker->fail();
    }
}

void Test::fail(const std::string message)
{
    print("Failed test with message: " + message);

    tracker->fail();
}
