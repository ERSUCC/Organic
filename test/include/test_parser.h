#pragma once

#include "exception.h"
#include "otest.h"
#include "parse.h"
#include "path.h"
#include "test.h"

struct TestParser : public Test
{
    TestParser(TestTracker* tracker);

    void test() override;

private:
    void expectError(const OTest* info);

};
