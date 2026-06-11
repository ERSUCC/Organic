#pragma once

#include "path.h"
#include "test.h"
#include "tokenize.h"

struct TestTokenizer : public Test
{
    TestTokenizer(TestTracker* tracker);

    void test() override;

private:
    void checkList(const Path* path);

};
