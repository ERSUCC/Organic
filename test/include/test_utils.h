#pragma once

#include <iostream>
#include <stack>
#include <stddef.h>
#include <stdlib.h>
#include <string>
#include <string.h>

struct TestUtils
{
    static void printSuccess(const std::string& text, const size_t indents = 0);
    static void printError(const std::string& text, const size_t indents = 0);

private:
    static void printIndents(const size_t indents);

};

struct TestTracker
{
    void beginSection();
    void fail();

    size_t endSection();

private:
    std::stack<size_t> failures;

};
