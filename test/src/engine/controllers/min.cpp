#include "engine/test_controllers.h"

void TestControllers::testMin()
{
    beginTest("Min", true);

    expectConstant(new Min(new List({ new Value(0) })), 0);
    expectConstant(new Min(new List({ new Value(5) })), 5);
    expectConstant(new Min(new List({ new Value(-5) })), -5);
    expectConstant(new Min(new List({ new Value(0), new Value(0), new Value(0) })), 0);
    expectConstant(new Min(new List({ new Value(5), new Value(5), new Value(5) })), 5);
    expectConstant(new Min(new List({ new Value(0), new Value(3), new Value(5) })), 0);
    expectConstant(new Min(new List({ new Value(0), new Value(-3), new Value(-5) })), -5);
    expectConstant(new Min(new List({ new Value(5), new Value(0), new Value(3) })), 0);

    expectValues(new Min(new List({ new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5) })),
    {
        TimeValue(0, 0),
        TimeValue(250, 1.25),
        TimeValue(500, 2.5),
        TimeValue(750, 2.5),
        TimeValue(1000, 0)
    });

    endTest();
}
