#pragma once

#include <string>
#include <vector>

#include "exception.h"
#include "path.h"
#include "test_utils.h"

struct Test
{
    Test(TestTracker* tracker);

    virtual void test() = 0;

protected:
    const Path* sourcePath(const std::string file) const;

    void beginSuite(const std::string name) const;

    void beginTest(const TestInfo* info);
    void endTest();

    void assert(const std::string name, const bool result);
    void fail(const std::string message);

private:
    void addError(const std::string text);

    TestTracker* tracker;

    const TestInfo* current;

    std::vector<std::string> errors;

};
