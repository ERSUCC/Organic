#include "engine/test_controllers.h"

void TestControllers::testHold()
{
    beginTest("Hold", true);

    expectConstantUntil(new Hold(new Value(0), new Value(0)), 0, 0);
    expectConstantUntil(new Hold(new Value(5), new Value(0)), 0, 0);
    expectConstantUntil(new Hold(new Value(0), new Value(1000)), 0, 1000);
    expectConstantUntil(new Hold(new Value(5), new Value(1000)), 5, 1000);

    endTest();
}
