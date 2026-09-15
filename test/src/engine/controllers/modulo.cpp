#include "engine/test_controllers.h"

void TestControllers::testModulo()
{
    beginTest("Modulo", true);

    expectConstant(new Modulo(new Value(0), new Value(0)), 0);
    expectConstant(new Modulo(new Value(0), new Value(5)), 0);
    expectConstant(new Modulo(new Value(0), new Value(-5)), 0);
    expectConstant(new Modulo(new Value(5), new Value(0)), 0);
    expectConstant(new Modulo(new Value(-5), new Value(0)), 0);
    expectConstant(new Modulo(new Value(5), new Value(1)), 0);
    expectConstant(new Modulo(new Value(-5), new Value(1)), 0);
    expectConstant(new Modulo(new Value(1), new Value(5)), 1);
    expectConstant(new Modulo(new Value(1), new Value(-5)), 1);

    expectValues(new Modulo(new Sweep(new Value(0), new Value(10), new Value(1000)), new Value(5)),
    {
        TimeValue(0, 0),
        TimeValue(100, 1),
        TimeValue(200, 2),
        TimeValue(300, 3),
        TimeValue(400, 4),
        TimeValue(500, 0),
        TimeValue(600, 1),
        TimeValue(700, 2),
        TimeValue(800, 3),
        TimeValue(900, 4),
        TimeValue(1000, 0)
    });

    expectValues(new Modulo(new Sweep(new Value(-5), new Value(5), new Value(1000)), new Value(5)),
    {
        TimeValue(0, 0),
        TimeValue(250, -2.5),
        TimeValue(500, 0),
        TimeValue(750, 2.5),
        TimeValue(1000, 0)
    });

    expectValues(new Modulo(new Sweep(new Value(5), new Value(-5), new Value(1000)), new Value(5)),
    {
        TimeValue(0, 0),
        TimeValue(250, 2.5),
        TimeValue(500, 0),
        TimeValue(750, -2.5),
        TimeValue(1000, 0)
    });

    endTest();
}
