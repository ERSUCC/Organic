#include "engine/test_controllers.h"

void TestControllers::testMax()
{
    beginTest("Max", true);

    expectConstant(new Max(new List({ new Value(0) })), 0);
    expectConstant(new Max(new List({ new Value(5) })), 5);
    expectConstant(new Max(new List({ new Value(-5) })), -5);
    expectConstant(new Max(new List({ new Value(0), new Value(0), new Value(0) })), 0);
    expectConstant(new Max(new List({ new Value(5), new Value(5), new Value(5) })), 5);
    expectConstant(new Max(new List({ new Value(0), new Value(3), new Value(5) })), 5);
    expectConstant(new Max(new List({ new Value(0), new Value(-3), new Value(-5) })), 0);
    expectConstant(new Max(new List({ new Value(0), new Value(5), new Value(3) })), 5);

    expectValues(new Max(new List({ new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5) })),
    {
        TimeValue(0, 2.5),
        TimeValue(250, 2.5),
        TimeValue(500, 2.5),
        TimeValue(750, 3.75),
        TimeValue(1000, 0)
    });

    endTest();
}
