#pragma once

#include "otest.h"
#include "path.h"
#include "source.h"
#include "test.h"
#include "tokenize.h"

struct TestTokenizer : public Test
{
    TestTokenizer(TestTracker* tracker);

    void test() override;

private:
    void checkList(const Path* path);
    void expectError(const Path* path);

};
