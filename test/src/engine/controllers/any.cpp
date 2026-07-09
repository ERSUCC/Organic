#include "engine/test_controllers.h"

void TestControllers::testAny()
{
    beginTest("Any", true);

    expectConstant(new Any(new List({ new Value(0) })), 0);
    expectConstant(new Any(new List({ new Value(5) })), 1);
    expectConstant(new Any(new List({ new Value(0), new Value(0), new Value(0) })), 0);
    expectConstant(new Any(new List({ new Value(0), new Value(0), new Value(5) })), 1);
    expectConstant(new Any(new List({ new Value(5), new Value(0), new Value(0) })), 1);
    expectConstant(new Any(new List({ new Value(5), new Value(5), new Value(5) })), 1);

    expectValues(new Any(new List({ new Value(5), new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)) })),
    {
        TimeValue(0, 1),
        TimeValue(250, 1),
        TimeValue(500, 1),
        TimeValue(750, 1),
        TimeValue(1000, 1)
    });

    expectValues(new Any(new List({ new ValueGreaterEqual(new Sweep(new Value(5), new Value(0), new Value(1000)), new Value(2.5)), new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)) })),
    {
        TimeValue(0, 1),
        TimeValue(250, 1),
        TimeValue(500, 1),
        TimeValue(750, 1),
        TimeValue(1000, 0)
    });

    endTest();
}
