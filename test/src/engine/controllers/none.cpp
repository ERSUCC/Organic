#include "engine/test_controllers.h"

void TestControllers::testNone()
{
    beginTest("None", true);

    expectConstant(new None(new List({ new Value(0) })), 1);
    expectConstant(new None(new List({ new Value(5) })), 0);
    expectConstant(new None(new List({ new Value(0), new Value(0), new Value(0) })), 1);
    expectConstant(new None(new List({ new Value(0), new Value(0), new Value(5) })), 0);
    expectConstant(new None(new List({ new Value(5), new Value(0), new Value(0) })), 0);
    expectConstant(new None(new List({ new Value(5), new Value(5), new Value(5) })), 0);

    endTest();
}
