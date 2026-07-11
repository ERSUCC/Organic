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

protected:
    Test(TestTracker* tracker);

    virtual void test() = 0;

    const Path sourcePath(const std::string file) const;
    const Path testPath(const std::string file) const;

    void beginSuite(const std::string name) const;

    void beginTest(const OTest* info);
    void beginTest(const std::string& name, const bool warn);
    void endTest();

    void fail(const std::string message);
    void failWithError(const OrganicException& error);
    void failAndCompare(const OTest* info, const OrganicException& error);
    void expectParseError(const OTest* info, const OrganicParseException& error);

private:
    void addError(const std::string text);

    TestTracker* tracker;

    std::vector<std::string> errors;

    std::string currentTest;

};
