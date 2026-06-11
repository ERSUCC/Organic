#pragma once

#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include "exception.h"
#include "path.h"

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

struct Test
{
    Test(TestTracker* tracker);

    virtual void test() = 0;

protected:
    const Path* sourcePath(const std::string file) const;

    void print(const std::string text);

    void beginSuite(const std::string name);
    void endSuite();

    void beginTest(const TestInfo* info);
    void endTest();

    void assert(const std::string name, const bool result);
    void fail(const std::string message);

    TestTracker* tracker;

    size_t indents = 0;

};
