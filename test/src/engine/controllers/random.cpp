#include "engine/test_controllers.h"

void TestControllers::testRandom()
{
    beginTest("Random", true);

    expectConstantUntil(new Random(new Value(0), new Value(0), new Value(0), new ValueByte(Constants::Random::Step)), 0, 0);
    expectConstantUntil(new Random(new Value(5), new Value(5), new Value(0), new ValueByte(Constants::Random::Step)), 0, 0);
    expectConstantUntil(new Random(new Value(0), new Value(0), new Value(1000), new ValueByte(Constants::Random::Step)), 0, 1000);
    expectConstantUntil(new Random(new Value(5), new Value(5), new Value(1000), new ValueByte(Constants::Random::Step)), 5, 1000);

    expectValues(new Random(new Value(0), new Value(5), new Value(1000), new ValueByte(Constants::Random::Step)),
    {
        TimeValue(0, 0.798967),
        TimeValue(250, 0.798967),
        TimeValue(500, 0.798967),
        TimeValue(750, 0.798967),
        TimeValue(1000, 0)
    }, 1e-6);

    expectValues(new Random(new Value(5), new Value(0), new Value(1000), new ValueByte(Constants::Random::Step)),
    {
        TimeValue(0, 4.201033),
        TimeValue(250, 4.201033),
        TimeValue(500, 4.201033),
        TimeValue(750, 4.201033),
        TimeValue(1000, 0)
    }, 1e-6);

    expectValues(new Random(new Value(0), new Value(-5), new Value(1000), new ValueByte(Constants::Random::Step)),
    {
        TimeValue(0, -0.798967),
        TimeValue(250, -0.798967),
        TimeValue(500, -0.798967),
        TimeValue(750, -0.798967),
        TimeValue(1000, 0)
    }, 1e-6);

    expectValues(new Repeat(new Random(new Value(0), new Value(5), new Value(1000), new ValueByte(Constants::Random::Step)), new Value(2)),
    {
        TimeValue(0, 0.798967),
        TimeValue(250, 0.798967),
        TimeValue(500, 0.798967),
        TimeValue(750, 0.798967),
        TimeValue(1000, 4.960726),
        TimeValue(1250, 4.960726),
        TimeValue(1500, 4.960726),
        TimeValue(1750, 4.960726),
        TimeValue(2000, 0)
    }, 1e-6);

    expectConstantUntil(new Random(new Value(0), new Value(0), new Value(0), new ValueByte(Constants::Random::Linear)), 0, 0);
    expectConstantUntil(new Random(new Value(5), new Value(5), new Value(0), new ValueByte(Constants::Random::Linear)), 0, 0);
    expectConstantUntil(new Random(new Value(0), new Value(0), new Value(1000), new ValueByte(Constants::Random::Linear)), 0, 1000);
    expectConstantUntil(new Random(new Value(5), new Value(5), new Value(1000), new ValueByte(Constants::Random::Linear)), 5, 1000);

    expectValues(new Random(new Value(0), new Value(5), new Value(1000), new ValueByte(Constants::Random::Linear)),
    {
        TimeValue(0, 0.798967),
        TimeValue(250, 1.839407),
        TimeValue(500, 2.879846),
        TimeValue(750, 3.920286),
        TimeValue(1000, 0)
    }, 1e-6);

    expectValues(new Random(new Value(5), new Value(0), new Value(1000), new ValueByte(Constants::Random::Linear)),
    {
        TimeValue(0, 4.201033),
        TimeValue(250, 3.160593),
        TimeValue(500, 2.120154),
        TimeValue(750, 1.079714),
        TimeValue(1000, 0)
    }, 1e-6);

    expectValues(new Random(new Value(0), new Value(-5), new Value(1000), new ValueByte(Constants::Random::Linear)),
    {
        TimeValue(0, -0.798967),
        TimeValue(250, -1.839407),
        TimeValue(500, -2.879846),
        TimeValue(750, -3.920286),
        TimeValue(1000, 0)
    }, 1e-6);

    expectValues(new Repeat(new Random(new Value(0), new Value(5), new Value(1000), new ValueByte(Constants::Random::Linear)), new Value(2)),
    {
        TimeValue(0, 0.798967),
        TimeValue(250, 1.839407),
        TimeValue(500, 2.879846),
        TimeValue(750, 3.920286),
        TimeValue(1000, 4.960726),
        TimeValue(1250, 3.770006),
        TimeValue(1500, 2.579286),
        TimeValue(1750, 1.388565),
        TimeValue(2000, 0)
    }, 1e-6);

    endTest();
}
