#include "../include/test_utils.h"

void TestUtils::printSuccess(const std::string& text, const size_t indents)
{
    std::cout << "\x1b[38;2;75;200;150m";

    printIndents(indents);

    std::cout << text << "\x1b[49;;m\n";
}

void TestUtils::printError(const std::string& text, const size_t indents)
{
    std::cout << "\x1b[38;2;255;128;128m";

    printIndents(indents);

    std::cout << text << "\x1b[49;;m\n";
}

void TestUtils::printIndents(const size_t indents)
{
    if (indents > 0)
    {
        for (size_t i = 0; i < indents; i++)
        {
            std::cout << "  ";
        }

        std::cout << "- ";
    }
}

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
