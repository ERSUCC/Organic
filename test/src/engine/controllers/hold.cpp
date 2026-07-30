#include "engine/test_controllers.h"

void TestControllers::testHold()
{
    beginTest("Hold (number)", true);

    expectConstantUntil(new Hold(new Value(0), new Value(0)), 0, 0);
    expectConstantUntil(new Hold(new Value(5), new Value(0)), 0, 0);
    expectConstantUntil(new Hold(new Value(0), new Value(1000)), 0, 1000);
    expectConstantUntil(new Hold(new Value(5), new Value(1000)), 5, 1000);

    endTest();

    beginTest("Hold (constant)", true);

    expectConstantUntil(new Hold(new ValueChar(0), new Value(0)), compareChar(0), 0, compareChar(0));
    expectConstantUntil(new Hold(new ValueChar(5), new Value(0)), compareChar(5), 0, compareChar(0));
    expectConstantUntil(new Hold(new ValueChar(0), new Value(1000)), compareChar(0), 1000, compareChar(0));
    expectConstantUntil(new Hold(new ValueChar(5), new Value(1000)), compareChar(5), 1000, compareChar(0));

    endTest();

    beginTest("Hold (list)", true);

    expectConstantUntil(new Hold(new List({ new Value(0) }), new Value(0)), compareList({ 0 }), 0, compareList({}));
    expectConstantUntil(new Hold(new List({ new Value(5) }), new Value(0)), compareList({ 5 }), 0, compareList({}));
    expectConstantUntil(new Hold(new List({ new Value(0) }), new Value(1000)), compareList({ 0 }), 1000, compareList({}));
    expectConstantUntil(new Hold(new List({ new Value(5) }), new Value(1000)), compareList({ 5 }), 1000, compareList({}));

    endTest();
}
