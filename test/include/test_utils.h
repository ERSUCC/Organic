#pragma once

#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include "exception.h"
#include "path.h"

struct TestUtils
{
    static void printSuccess(const std::string& text, const size_t indents = 0);
    static void printError(const std::string& text, const size_t indents = 0);

private:
    static void printIndents(const size_t indents);

};

struct TestInfo
{
    TestInfo(const Path* path);

    std::string name() const;

protected:
    std::vector<std::vector<std::string>> data;

};

struct TestErrorInfo : public TestInfo
{
    TestErrorInfo(const Path* path);

    bool matches(const OrganicParseException& error) const;

private:
    const size_t line;
    const size_t character;

    const std::string& message;

};

struct TestTokenizerInfo : public TestInfo
{
    TestTokenizerInfo(const Path* path);

    const std::vector<std::string>& tokens() const;
};

struct TestTracker
{
    void beginSection();
    void fail();

    size_t endSection();

private:
    std::stack<size_t> failures;

};
