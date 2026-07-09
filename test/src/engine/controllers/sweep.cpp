#include "engine/test_controllers.h"

void TestControllers::testSweep()
{
    beginTest("Sweep", true);

    expectConstantUntil(new Sweep(new Value(0), new Value(0), new Value(0)), 0, 0);
    expectConstantUntil(new Sweep(new Value(5), new Value(5), new Value(0)), 0, 0);
    expectConstantUntil(new Sweep(new Value(0), new Value(5), new Value(0)), 0, 0);
    expectConstantUntil(new Sweep(new Value(5), new Value(0), new Value(0)), 0, 0);
    expectConstantUntil(new Sweep(new Value(0), new Value(0), new Value(1000)), 0, 1000);
    expectConstantUntil(new Sweep(new Value(5), new Value(5), new Value(1000)), 5, 1000);

    expectValues(new Sweep(new Value(0), new Value(5), new Value(1000)),
    {
        TimeValue(0, 0),
        TimeValue(250, 1.25),
        TimeValue(500, 2.5),
        TimeValue(750, 3.75),
        TimeValue(1000, 0)
    });

    expectValues(new Sweep(new Value(5), new Value(0), new Value(1000)),
    {
        TimeValue(0, 5),
        TimeValue(250, 3.75),
        TimeValue(500, 2.5),
        TimeValue(750, 1.25),
        TimeValue(1000, 0)
    });

    expectValues(new Sweep(new Value(0), new Value(-5), new Value(1000)),
    {
        TimeValue(0, 0),
        TimeValue(250, -1.25),
        TimeValue(500, -2.5),
        TimeValue(750, -3.75),
        TimeValue(1000, 0)
    });

    expectValues(new Sweep(new Value(-5), new Value(0), new Value(1000)),
    {
        TimeValue(0, -5),
        TimeValue(250, -3.75),
        TimeValue(500, -2.5),
        TimeValue(750, -1.25),
        TimeValue(1000, 0)
    });

    endTest();
}
