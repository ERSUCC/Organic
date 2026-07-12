#include "engine/test_controllers.h"

using namespace Engine;

TimeValue::TimeValue(const double time, const double value) :
    time(time), value(value) {}

void TestControllers::run(TestTracker* tracker)
{
    TestControllers* test = new TestControllers(tracker);

    test->test();

    delete test;
}

void TestControllers::test()
{
    beginSuite("Test controllers");

    testValue();
    testValueNegate();
    testValueAdd();
    testValueSubtract();
    testValueSubtract();
    testValueMultiply();
    testValueDivide();
    testValuePower();
    testValueEquals();
    testValueLess();
    testValueGreater();
    testValueLessEqual();
    testValueGreaterEqual();
    testTime();
    testHold();
    testLFO();
    testSweep();
    testSequence();
    testRepeat();
    testRandom();
    testLimit();
    testTrigger();
    testIf();
    testAll();
    testAny();
    testNone();
    testMin();
    testMax();
    testRound();
    testAbsolute();
}

TestControllers::TestControllers(TestTracker* tracker) :
    Test(tracker), utils(Utils::get()) {}

void TestControllers::expectValues(ValueObject* object, const std::vector<TimeValue>& values, const double epsilon)
{
    std::unique_ptr<ValueObject> unique(object);

    expectValues(unique, values, epsilon);
}

void TestControllers::expectConstant(ValueObject* object, const double value, const double epsilon)
{
    std::unique_ptr<ValueObject> unique(object);

    expectValues(unique,
    {
        TimeValue(0, value),
        TimeValue(250, value),
        TimeValue(500, value),
        TimeValue(750, value),
        TimeValue(1000, value)
    }, epsilon);

    if (!unique->enabled)
    {
        fail("Expected object to continue at time 1000, but it stopped.");
    }
}

void TestControllers::expectConstantUntil(ValueObject* object, const double value, const double end, const double epsilon)
{
    std::unique_ptr<ValueObject> unique(object);

    std::vector<TimeValue> expected;

    for (double time = 0; time < end; time += 250)
    {
        expected.emplace_back(time, value);
    }

    expected.emplace_back(end, 0);

    expectValues(unique, expected, epsilon);

    if (unique->enabled)
    {
        fail("Expected object to stop at time " + formatDouble(end) + ", but it continued.");
    }
}

void TestControllers::expectValues(std::unique_ptr<ValueObject>& object, const std::vector<TimeValue>& values, const double epsilon)
{
    utils->setSeed(0);

    utils->time = 0;

    for (const TimeValue& value : values)
    {
        object->start(utils->time);

        while (utils->time < value.time)
        {
            object->getValue();

            utils->time += utils->timeStep;
        }

        utils->time = value.time;

        const double actual = object->getValue();

        if (fabs(actual - value.value) >= epsilon)
        {
            fail("Expected " + formatDouble(value.value) + " at time " + formatDouble(value.time) + ", but received " + formatDouble(actual));
        }
    }
}

std::string TestControllers::formatDouble(const double value) const
{
    char buffer[32];

    int end = snprintf(buffer, 32, "%f", value);

    while (end > 0 && buffer[end - 1] == '0')
    {
        end--;
    }

    if (end > 0 && buffer[end - 1] == '.')
    {
        end--;
    }

    if (end >= 0)
    {
        buffer[end] = '\0';
    }

    return buffer;
}
