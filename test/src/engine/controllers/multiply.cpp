#include "engine/test_controllers.h"

void TestControllers::testValueMultiply()
{
    beginTest("ValueMultiply", true);

    expectConstant(new ValueMultiply(new Value(0), new Value(0)), 0);
    expectConstant(new ValueMultiply(new Value(5), new Value(0)), 0);
    expectConstant(new ValueMultiply(new Value(0), new Value(5)), 0);
    expectConstant(new ValueMultiply(new Value(5), new Value(5)), 25);
    expectConstant(new ValueMultiply(new Value(-5), new Value(-5)), 25);
    expectConstant(new ValueMultiply(new Value(5), new Value(-5)), -25);
    expectConstant(new ValueMultiply(new Value(-5), new Value(5)), -25);

    expectValues(new ValueMultiply(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(0), new Value(5), new Value(1000))),
    {
        TimeValue(0, 0),
        TimeValue(250, 1.5625),
        TimeValue(500, 6.25),
        TimeValue(750, 14.0625),
        TimeValue(1000, 0)
    });

    expectValues(new ValueMultiply(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(5), new Value(0), new Value(1000))),
    {
        TimeValue(0, 0),
        TimeValue(250, 4.6875),
        TimeValue(500, 6.25),
        TimeValue(750, 4.6875),
        TimeValue(1000, 0)
    });

    endTest();
}
