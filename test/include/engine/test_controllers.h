#pragma once

#include <limits>
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

#include "constants.h"
#include "controller.h"
#include "object.h"

#include "../test.h"
#include "../test_utils.h"

using namespace Engine;

struct TimeValue
{
    TimeValue(const double time, const double value);

    const double time;
    const double value;
};

struct TestControllers : public Test
{
    static void run(TestTracker* tracker);

protected:
    void test() override;

private:
    TestControllers(TestTracker* tracker);

    void testValue();
    void testValueAdd();
    void testValueSubtract();
    void testValueMultiply();
    void testValueDivide();
    void testValuePower();
    void testValueEquals();
    void testValueLess();
    void testValueGreater();
    void testValueLessEqual();
    void testValueGreaterEqual();
    void testTime();
    void testHold();
    void testLFO();
    void testSweep();
    void testSequence();
    void testRepeat();
    void testRandom();
    void testLimit();
    void testTrigger();
    void testIf();
    void testAll();
    void testAny();
    void testNone();
    void testMin();
    void testMax();
    void testRound();
    void testAbsolute();

    void expectValues(ValueObject* object, const std::vector<TimeValue>& values, const double epsilon = std::numeric_limits<double>::epsilon());
    void expectConstant(ValueObject* object, const double value, const double epsilon = std::numeric_limits<double>::epsilon());
    void expectConstantUntil(ValueObject* object, const double value, const double end, const double epsilon = std::numeric_limits<double>::epsilon());

    void expectValues(std::unique_ptr<ValueObject>& object, const std::vector<TimeValue>& values, const double epsilon);

    std::string formatDouble(const double value) const;

    Utils* utils;

};
