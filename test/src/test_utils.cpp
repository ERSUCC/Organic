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

    char* data = (char*)malloc(sizeof(char) * text.size() + 1);

    strncpy(data, text.c_str(), text.size() + 1);

    char* next = strtok(data, "\n");

    while (next)
    {
        printIndents(indents);

        std::cout << next << "\n";

        next = strtok(nullptr, "\n");
    }

    free(data);

    std::cout << "\x1b[49;;m";
}

std::string TestUtils::formatDouble(const double value)
{
    char buffer[32];

    int end = snprintf(buffer, 32, "%f", value);

    while (end > 0 && buffer[end - 1] == '0')
    {
        end--;
    }

    if (end > 0 && buffer[end - 1] == '.')
    {
        end--;
    }

    if (end >= 0)
    {
        buffer[end] = '\0';
    }

    return buffer;
}

void TestUtils::printIndents(const size_t indents)
{
    if (indents > 0)
    {
        for (size_t i = 0; i < indents; i++)
        {
            std::cout << "  ";
        }

        std::cout << "| ";
    }
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
