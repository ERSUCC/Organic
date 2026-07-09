#include "engine/test_controllers.h"

void TestControllers::testRepeat()
{
    beginTest("Repeat", true);

    expectConstant(new Repeat(new Value(0), new Value(0)), 0);
    expectConstant(new Repeat(new Value(0), new Value(1)), 0);
    expectConstant(new Repeat(new Value(0), new Value(5)), 0);
    expectConstant(new Repeat(new Hold(new Value(0), new Value(1000)), new Value(0)), 0);
    expectConstantUntil(new Repeat(new Hold(new Value(0), new Value(1000)), new Value(1)), 0, 1000);
    expectConstantUntil(new Repeat(new Hold(new Value(0), new Value(1000)), new Value(5)), 0, 5000);
    expectConstant(new Repeat(new Hold(new Value(5), new Value(1000)), new Value(0)), 5);
    expectConstantUntil(new Repeat(new Hold(new Value(5), new Value(1000)), new Value(1)), 5, 1000);
    expectConstantUntil(new Repeat(new Hold(new Value(5), new Value(1000)), new Value(5)), 5, 5000);

    endTest();
}
