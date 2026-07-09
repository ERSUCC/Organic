#include "engine/test_controllers.h"

void TestControllers::testLFO()
{
    beginTest("LFO", true);

    expectConstantUntil(new LFO(new Value(0), new Value(0), new Value(0)), 0, 0);
    expectConstantUntil(new LFO(new Value(5), new Value(5), new Value(0)), 0, 0);
    expectConstantUntil(new LFO(new Value(0), new Value(0), new Value(1000)), 0, 1000);
    expectConstantUntil(new LFO(new Value(5), new Value(5), new Value(1000)), 5, 1000);

    expectValues(new LFO(new Value(0), new Value(5), new Value(1000)),
    {
        TimeValue(0, 0),
        TimeValue(250, 2.5),
        TimeValue(500, 5),
        TimeValue(750, 2.5),
        TimeValue(1000, 0)
    }, 1e-10);

    expectValues(new LFO(new Value(5), new Value(0), new Value(1000)),
    {
        TimeValue(0, 5),
        TimeValue(250, 2.5),
        TimeValue(500, 0),
        TimeValue(750, 2.5),
        TimeValue(1000, 0)
    }, 1e-10);

    expectValues(new LFO(new Value(0), new Value(-5), new Value(1000)),
    {
        TimeValue(0, 0),
        TimeValue(250, -2.5),
        TimeValue(500, -5),
        TimeValue(750, -2.5),
        TimeValue(1000, 0)
    }, 1e-10);

    endTest();
}
