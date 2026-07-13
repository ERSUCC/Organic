#pragma once

#include <limits>

#include "exception.h"
#include "otest.h"
#include "parse.h"
#include "path.h"
#include "source.h"
#include "test.h"
#include "test_utils.h"
#include "token.h"
#include "transform.h"

struct TestValue : public Test
{
    static void run(TestTracker* tracker);

protected:
    void test() override;

private:
    TestValue(TestTracker* tracker);

    void expectValue(const OTest* info);

};
