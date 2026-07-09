#include "engine/test_controllers.h"

void TestControllers::testValue()
{
    beginTest("Value", true);

    expectConstant(new Value(0), 0);
    expectConstant(new Value(5), 5);
    expectConstant(new Value(-5), -5);

    endTest();
}
