#include "engine/test_controllers.h"

void TestControllers::testValuePower()
{
    beginTest("ValuePower", true);

    expectConstant(new ValuePower(new Value(0), new Value(0)), 1);
    expectConstant(new ValuePower(new Value(5), new Value(0)), 1);
    expectConstant(new ValuePower(new Value(0), new Value(5)), 0);
    expectConstant(new ValuePower(new Value(5), new Value(5)), 3125);
    expectConstant(new ValuePower(new Value(-5), new Value(-5)), -0.00032);
    expectConstant(new ValuePower(new Value(5), new Value(-5)), 0.00032);
    expectConstant(new ValuePower(new Value(-5), new Value(5)), -3125);

    expectValues(new ValuePower(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(0), new Value(5), new Value(1000))),
    {
        TimeValue(0, 1),
        TimeValue(250, 1.321714),
        TimeValue(500, 9.882118),
        TimeValue(750, 142.107583),
        TimeValue(1000, 0)
    }, 1e-6);

    expectValues(new ValuePower(new Sweep(new Value(0), new Value(5), new Value(1000)), new Sweep(new Value(5), new Value(0), new Value(1000))),
    {
        TimeValue(0, 0),
        TimeValue(250, 2.308939),
        TimeValue(500, 9.882118),
        TimeValue(750, 5.218421),
        TimeValue(1000, 0)
    }, 1e-6);

    endTest();
}
