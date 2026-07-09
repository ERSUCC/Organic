#include "engine/test_controllers.h"

void TestControllers::testValueDivide()
{
    beginTest("ValueDivide", true);

    expectConstant(new ValueDivide(new Value(0), new Value(1)), 0);
    expectConstant(new ValueDivide(new Value(0), new Value(5)), 0);
    expectConstant(new ValueDivide(new Value(1), new Value(1)), 1);
    expectConstant(new ValueDivide(new Value(5), new Value(1)), 5);
    expectConstant(new ValueDivide(new Value(1), new Value(5)), 0.2);
    expectConstant(new ValueDivide(new Value(5), new Value(5)), 1);
    expectConstant(new ValueDivide(new Value(-5), new Value(-5)), 1);
    expectConstant(new ValueDivide(new Value(5), new Value(-5)), -1);
    expectConstant(new ValueDivide(new Value(-5), new Value(5)), -1);

    expectValues(new ValueDivide(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(5)),
    {
        TimeValue(0, 0),
        TimeValue(200, 0.2),
        TimeValue(400, 0.4),
        TimeValue(600, 0.6),
        TimeValue(800, 0.8),
        TimeValue(1000, 0),
    });

    endTest();
}
