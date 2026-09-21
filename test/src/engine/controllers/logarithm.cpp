#include "engine/test_controllers.h"

void TestControllers::testLogarithm()
{
    beginTest("Logarithm", true);

    expectConstant(new Logarithm(new Value(0), new Value(0)), 0);
    expectConstant(new Logarithm(new Value(0), new Value(5)), 0);
    expectConstant(new Logarithm(new Value(0), new Value(-5)), 0);
    expectConstant(new Logarithm(new Value(5), new Value(0)), 0);
    expectConstant(new Logarithm(new Value(-5), new Value(0)), 0);
    expectConstant(new Logarithm(new Value(5), new Value(1)), 0);
    expectConstant(new Logarithm(new Value(-5), new Value(1)), 0);
    expectConstant(new Logarithm(new Value(1), new Value(5)), 0);
    expectConstant(new Logarithm(new Value(1), new Value(-5)), 0);
    expectConstant(new Logarithm(new Value(5), new Value(5)), 1);
    expectConstant(new Logarithm(new Value(25), new Value(5)), 2);
    expectConstant(new Logarithm(new Value(5), new Value(25)), 0.5);

    expectValues(new Logarithm(new ValuePower(new Value(10), new Sweep(new Value(0), new Value(10), new Value(1000))), new Value(10)),
    {
        TimeValue(0, 0),
        TimeValue(100, 1),
        TimeValue(200, 2),
        TimeValue(300, 3),
        TimeValue(400, 4),
        TimeValue(500, 5),
        TimeValue(600, 6),
        TimeValue(700, 7),
        TimeValue(800, 8),
        TimeValue(900, 9),
        TimeValue(1000, 0)
    }, 1e-6);

    expectValues(new Logarithm(new ValuePower(new Value(10), new Sweep(new Value(10), new Value(0), new Value(1000))), new Value(10)),
    {
        TimeValue(0, 10),
        TimeValue(100, 9),
        TimeValue(200, 8),
        TimeValue(300, 7),
        TimeValue(400, 6),
        TimeValue(500, 5),
        TimeValue(600, 4),
        TimeValue(700, 3),
        TimeValue(800, 2),
        TimeValue(900, 1),
        TimeValue(1000, 0)
    }, 1e-6);

    endTest();
}
