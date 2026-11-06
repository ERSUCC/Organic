#pragma once

#include <filesystem>
#include <time.h>

#include "exception.h"
#include "flags.h"
#include "organic.h"
#include "path.h"
#include "test.h"
#include "test_utils.h"

struct TestExamples : public Test
{
    static void run(TestTracker* tracker);

protected:
    void test() override;

private:
    TestExamples(TestTracker* tracker);

    void expectSuccess(const Path& path);

};
