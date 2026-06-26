#pragma once

#include "otest.h"
#include "path.h"
#include "source.h"
#include "test.h"
#include "test_utils.h"
#include "tokenize.h"

struct TestTokenizer : public Test
{
    static void run(TestTracker* tracker);

protected:
    void test() override;

private:
    TestTokenizer(TestTracker* tracker);

    void checkList(const OTest* info);
    void expectError(const OTest* info);

};
