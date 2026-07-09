#include "engine/test_controllers.h"

void TestControllers::testValueLess()
{
    beginTest("ValueLess", true);

    expectConstant(new ValueLess(new Value(0), new Value(0)), 0);
    expectConstant(new ValueLess(new Value(5), new Value(0)), 0);
    expectConstant(new ValueLess(new Value(0), new Value(5)), 1);
    expectConstant(new ValueLess(new Value(5), new Value(5)), 0);
    expectConstant(new ValueLess(new Value(-5), new Value(-5)), 0);
    expectConstant(new ValueLess(new Value(5), new Value(-5)), 0);
    expectConstant(new ValueLess(new Value(-5), new Value(5)), 1);

    expectValues(new ValueLess(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(0), new Value(5), new Value(1000))),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 0),
        TimeValue(750, 0),
        TimeValue(1000, 0)
    });

    expectValues(new ValueLess(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(5), new Value(0), new Value(1000))),
    {
        TimeValue(0, 1),
        TimeValue(250, 1),
        TimeValue(500, 0),
        TimeValue(750, 0),
        TimeValue(1000, 0)
    });

    endTest();
}
