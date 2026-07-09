#include "engine/test_controllers.h"

void TestControllers::testTrigger()
{
    beginTest("Trigger", true);

    expectConstant(new Trigger(new Value(0), new Value(0)), 0);
    expectConstant(new Trigger(new Value(0), new Value(5)), 0);
    expectConstant(new Trigger(new Value(1), new Value(0)), 0);
    expectConstant(new Trigger(new Value(1), new Value(5)), 5);

    expectValues(new Trigger(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new Value(5)),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 5),
        TimeValue(750, 5),
        TimeValue(1000, 5)
    });

    expectValues(new Trigger(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new Hold(new Value(5), new Value(1000))),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 5),
        TimeValue(750, 5),
        TimeValue(1000, 5),
        TimeValue(1250, 5),
        TimeValue(1500, 0)
    });

    endTest();
}
