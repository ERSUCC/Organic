#include "engine/test_controllers.h"

using namespace Engine;

TimeValue::TimeValue(const double time, const double value) :
    time(time), value(value) {}

TimeLambda::TimeLambda(const double time, const ValueFunc& function) :
    time(time), function(function) {}

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
        fail("Expected object to stop at time " + TestUtils::formatDouble(end) + ", but it continued.");
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
            object->update();

            utils->time += utils->timeStep;
        }

        utils->time = value.time;

        object->update();

        const double actual = object->getValue();

        if (fabs(actual - value.value) >= epsilon)
        {
            fail("Expected " + TestUtils::formatDouble(value.value) + " at time " + TestUtils::formatDouble(value.time) + ", but received " + TestUtils::formatDouble(actual));
        }
    }
}

void TestControllers::expectValues(ValueObject* object, const std::vector<TimeLambda>& values)
{
    std::unique_ptr<ValueObject> unique(object);

    expectValues(unique, values);
}

void TestControllers::expectConstant(ValueObject* object, const ValueFunc& function)
{
    std::unique_ptr<ValueObject> unique(object);

    expectValues(unique,
    {
        TimeLambda(0, function),
        TimeLambda(250, function),
        TimeLambda(500, function),
        TimeLambda(750, function),
        TimeLambda(1000, function)
    });

    if (!unique->enabled)
    {
        fail("Expected object to continue at time 1000, but it stopped.");
    }
}

void TestControllers::expectConstantUntil(ValueObject* object, const ValueFunc& function, const double end, const ValueFunc& endFunc)
{
    std::unique_ptr<ValueObject> unique(object);

    std::vector<TimeLambda> expected;

    for (double time = 0; time < end; time += 250)
    {
        expected.emplace_back(time, function);
    }

    expected.emplace_back(end, endFunc);

    expectValues(unique, expected);

    if (unique->enabled)
    {
        fail("Expected object to stop at time " + TestUtils::formatDouble(end) + ", but it continued.");
    }
}

void TestControllers::expectValues(std::unique_ptr<ValueObject>& object, const std::vector<TimeLambda>& values)
{
    utils->setSeed(0);

    utils->time = 0;

    for (const TimeLambda& value : values)
    {
        object->start(utils->time);

        while (utils->time < value.time)
        {
            object->update();

            utils->time += utils->timeStep;
        }

        utils->time = value.time;

        object->update();

        value.function(object.get(), value.time);
    }
}

ValueFunc TestControllers::compareChar(const unsigned char value)
{
    return [=](ValueObject* object, const double time)
    {
        const unsigned char actual = object->getLeafAs<ValueChar>()->value;

        if (actual != value)
        {
            fail("Expected " + TestUtils::formatDouble(value) + " at time " + TestUtils::formatDouble(time) + ", but received " + TestUtils::formatDouble(actual));
        }
    };
}

ValueFunc TestControllers::compareList(const std::vector<double>& values)
{
    return [&](ValueObject* object, const double time)
    {
        const std::vector<ValueObject*>& objects = object->getLeafAs<List>()->objects;

        if (objects.size() != values.size())
        {
            fail("Expected list of size " + std::to_string(values.size()) + " at time " + TestUtils::formatDouble(time) + ", but received list of size " + std::to_string(objects.size()));

            return;
        }

        for (size_t i = 0; i < objects.size(); i++)
        {
            const double actual = objects[i]->getValue();

            if (actual != values[i])
            {
                fail("Expected " + TestUtils::formatDouble(values[i]) + " at index " + std::to_string(i) + " in list at time " + TestUtils::formatDouble(time) + ", but received " + TestUtils::formatDouble(actual));

                return;
            }
        }
    };
}
