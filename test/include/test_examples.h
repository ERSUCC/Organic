#pragma once

#include "exception.h"
#include "parse.h"
#include "path.h"
#include "resolve.h"
#include "test.h"
#include "test_utils.h"
#include "token.h"
#include "transform.h"

struct TestExamples : public Test
{
    TestExamples(TestTracker* tracker);

    void test() override;

private:
    void expectSuccess(const Path* path);

};
