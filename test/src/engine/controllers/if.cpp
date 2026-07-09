#include "engine/test_controllers.h"

void TestControllers::testIf()
{
    beginTest("If", true);

    expectConstant(new If(new Value(0), new Value(0), new Value(0)), 0);
    expectConstant(new If(new Value(0), new Value(5), new Value(0)), 0);
    expectConstant(new If(new Value(1), new Value(0), new Value(0)), 0);
    expectConstant(new If(new Value(1), new Value(5), new Value(0)), 5);

    expectValues(new If(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new Value(5), new Value(0)),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 5),
        TimeValue(750, 5),
        TimeValue(1000, 0)
    });

    endTest();
}
