#pragma once

#include <stddef.h>
#include <string>
#include <vector>

#include "exception.h"
#include "otest.h"
#include "path.h"
#include "test_utils.h"
#include "utils.h"

struct Test
{
    Test(TestTracker* tracker);

    virtual void test() = 0;

protected:
    const Path* sourcePath(const std::string file) const;
    const Path* testPath(const std::string file) const;

    void beginSuite(const std::string name) const;

    void beginTest(const bool warn);
    void endTest(const std::string name);

    void assert(const std::string name, const bool result);
    void fail(const std::string message);

    bool matchParseError(const OTest* info, const OrganicParseException& error) const;

private:
    void addError(const std::string text);

    TestTracker* tracker;

    std::vector<std::string> errors;

};
