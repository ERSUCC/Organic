#include "engine/test_controllers.h"

void TestControllers::testValueSubtract()
{
    beginTest("ValueSubtract", true);

    expectConstant(new ValueSubtract(new Value(0), new Value(0)), 0);
    expectConstant(new ValueSubtract(new Value(5), new Value(0)), 5);
    expectConstant(new ValueSubtract(new Value(0), new Value(5)), -5);
    expectConstant(new ValueSubtract(new Value(5), new Value(5)), 0);
    expectConstant(new ValueSubtract(new Value(-5), new Value(-5)), 0);
    expectConstant(new ValueSubtract(new Value(5), new Value(-5)), 10);
    expectConstant(new ValueSubtract(new Value(-5), new Value(5)), -10);

    expectValues(new ValueSubtract(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(0), new Value(5), new Value(1000))),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 0),
        TimeValue(750, 0),
        TimeValue(1000, 0)
    });

    expectValues(new ValueSubtract(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(5), new Value(0), new Value(1000))),
    {
        TimeValue(0, -5),
        TimeValue(250, -2.5),
        TimeValue(500, 0),
        TimeValue(750, 2.5),
        TimeValue(1000, 0)
    });

    endTest();
}
