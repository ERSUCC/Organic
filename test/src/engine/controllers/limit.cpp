#include "engine/test_controllers.h"

void TestControllers::testLimit()
{
    beginTest("Limit", true);

    expectConstant(new Limit(new Value(0), new Value(0), new Value(0)), 0);
    expectConstant(new Limit(new Value(5), new Value(0), new Value(0)), 0);
    expectConstant(new Limit(new Value(-5), new Value(0), new Value(0)), 0);
    expectConstant(new Limit(new Value(0), new Value(0), new Value(5)), 0);
    expectConstant(new Limit(new Value(5), new Value(0), new Value(5)), 5);
    expectConstant(new Limit(new Value(-5), new Value(0), new Value(5)), 0);
    expectConstant(new Limit(new Value(0), new Value(-5), new Value(0)), 0);
    expectConstant(new Limit(new Value(5), new Value(-5), new Value(0)), 0);
    expectConstant(new Limit(new Value(-5), new Value(-5), new Value(0)), -5);

    expectValues(new Limit(new Sweep(new Value(-5), new Value(10), new Value(1000)), new Value(0), new Value(5)),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 2.5),
        TimeValue(750, 5),
        TimeValue(1000, 0)
    });

    expectValues(new Limit(new Sweep(new Value(-10), new Value(5), new Value(1000)), new Value(-5), new Value(0)),
    {
        TimeValue(0, -5),
        TimeValue(250, -5),
        TimeValue(500, -2.5),
        TimeValue(750, 0),
        TimeValue(1000, 0)
    });

    endTest();
}
