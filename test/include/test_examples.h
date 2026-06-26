#pragma once

#include "exception.h"
#include "parse.h"
#include "path.h"
#include "resolve.h"
#include "source.h"
#include "test.h"
#include "test_utils.h"
#include "token.h"
#include "transform.h"

struct TestExamples : public Test
{
    static void run(TestTracker* tracker);

protected:
    void test() override;

private:
    TestExamples(TestTracker* tracker);

    void expectSuccess(const Path* path);

};
