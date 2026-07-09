#include "engine/test_controllers.h"

void TestControllers::testValueAdd()
{
    beginTest("ValueAdd", true);

    expectConstant(new ValueAdd(new Value(0), new Value(0)), 0);
    expectConstant(new ValueAdd(new Value(5), new Value(0)), 5);
    expectConstant(new ValueAdd(new Value(0), new Value(5)), 5);
    expectConstant(new ValueAdd(new Value(5), new Value(5)), 10);
    expectConstant(new ValueAdd(new Value(-5), new Value(-5)), -10);
    expectConstant(new ValueAdd(new Value(5), new Value(-5)), 0);
    expectConstant(new ValueAdd(new Value(-5), new Value(5)), 0);

    expectValues(new ValueAdd(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(0), new Value(5), new Value(1000))),
    {
        TimeValue(0, 0),
        TimeValue(250, 2.5),
        TimeValue(500, 5),
        TimeValue(750, 7.5),
        TimeValue(1000, 0)
    });

    expectValues(new ValueAdd(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(5), new Value(0), new Value(1000))),
    {
        TimeValue(0, 5),
        TimeValue(250, 5),
        TimeValue(500, 5),
        TimeValue(750, 5),
        TimeValue(1000, 0)
    });

    endTest();
}
