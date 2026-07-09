#include "engine/test_controllers.h"

void TestControllers::testValueGreaterEqual()
{
    beginTest("ValueGreaterEqual", true);

    expectConstant(new ValueGreaterEqual(new Value(0), new Value(0)), 1);
    expectConstant(new ValueGreaterEqual(new Value(5), new Value(0)), 1);
    expectConstant(new ValueGreaterEqual(new Value(0), new Value(5)), 0);
    expectConstant(new ValueGreaterEqual(new Value(5), new Value(5)), 1);
    expectConstant(new ValueGreaterEqual(new Value(-5), new Value(-5)), 1);
    expectConstant(new ValueGreaterEqual(new Value(5), new Value(-5)), 1);
    expectConstant(new ValueGreaterEqual(new Value(-5), new Value(5)), 0);

    expectValues(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(0), new Value(5), new Value(1000))),
    {
        TimeValue(0, 1),
        TimeValue(250, 1),
        TimeValue(500, 1),
        TimeValue(750, 1),
        TimeValue(1000, 0)
    });

    expectValues(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(5), new Value(0), new Value(1000))),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 1),
        TimeValue(750, 1),
        TimeValue(1000, 0)
    });

    endTest();
}
