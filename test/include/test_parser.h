#pragma once

#include "exception.h"
#include "otest.h"
#include "parse.h"
#include "path.h"
#include "source.h"
#include "test.h"
#include "test_utils.h"

struct TestParser : public Test
{
    static void run(TestTracker* tracker);

protected:
    void test() override;

private:
    TestParser(TestTracker* tracker);

    void expectSuccess(const OTest* info);
    void expectError(const OTest* info);

};
