#include "engine/test_controllers.h"

void TestControllers::testValueNegate()
{
    beginTest("ValueNegate", true);

    expectConstant(new ValueNegate(new Value(0)), 0);
    expectConstant(new ValueNegate(new Value(5)), -5);
    expectConstant(new ValueNegate(new Value(-5)), 5);

    expectValues(new ValueNegate(new Sweep(new Value(5), new Value(-5), new Value(1000))),
    {
        TimeValue(0, -5),
        TimeValue(250, -2.5),
        TimeValue(500, 0),
        TimeValue(750, 2.5),
        TimeValue(1000, 0)
    });

    expectValues(new ValueNegate(new Sweep(new Value(-5), new Value(5), new Value(1000))),
    {
        TimeValue(0, 5),
        TimeValue(250, 2.5),
        TimeValue(500, 0),
        TimeValue(750, -2.5),
        TimeValue(1000, 0)
    });

    endTest();
}
