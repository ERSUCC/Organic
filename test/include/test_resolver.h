#pragma once

#include "exception.h"
#include "parse.h"
#include "path.h"
#include "test.h"
#include "types.h"

struct TestResolver : public Test
{
    TestResolver(TestTracker* tracker);

    void test() override;

private:
    void expectSuccess(const Path* path);
    void expectError(const Path* path);

};
