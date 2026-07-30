#include "engine/test_controllers.h"

void TestControllers::testRepeat()
{
    beginTest("Repeat (number)", true);

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

    beginTest("Repeat (constant)", true);

    expectConstant(new Repeat(new ValueChar(0), new Value(0)), compareChar(0));
    expectConstant(new Repeat(new ValueChar(0), new Value(1)), compareChar(0));
    expectConstant(new Repeat(new ValueChar(0), new Value(5)), compareChar(0));
    expectConstant(new Repeat(new Hold(new ValueChar(0), new Value(1000)), new Value(0)), compareChar(0));
    expectConstantUntil(new Repeat(new Hold(new ValueChar(0), new Value(1000)), new Value(1)), compareChar(0), 1000, compareChar(0));
    expectConstantUntil(new Repeat(new Hold(new ValueChar(0), new Value(1000)), new Value(5)), compareChar(0), 5000, compareChar(0));
    expectConstant(new Repeat(new Hold(new ValueChar(5), new Value(1000)), new Value(0)), compareChar(5));
    expectConstantUntil(new Repeat(new Hold(new ValueChar(5), new Value(1000)), new Value(1)), compareChar(5), 1000, compareChar(0));
    expectConstantUntil(new Repeat(new Hold(new ValueChar(5), new Value(1000)), new Value(5)), compareChar(5), 5000, compareChar(0));

    endTest();

    beginTest("Repeat (list)", true);

    expectConstant(new Repeat(new List({ new Value(0) }), new Value(0)), compareList({ 0 }));
    expectConstant(new Repeat(new List({ new Value(0) }), new Value(1)), compareList({ 0 }));
    expectConstant(new Repeat(new List({ new Value(0) }), new Value(5)), compareList({ 0 }));
    expectConstant(new Repeat(new Hold(new List({ new Value(0) }), new Value(1000)), new Value(0)), compareList({ 0 }));
    expectConstantUntil(new Repeat(new Hold(new List({ new Value(0) }), new Value(1000)), new Value(1)), compareList({ 0 }), 1000, compareList({}));
    expectConstantUntil(new Repeat(new Hold(new List({ new Value(0) }), new Value(1000)), new Value(5)), compareList({ 0 }), 5000, compareList({}));
    expectConstant(new Repeat(new Hold(new List({ new Value(5) }), new Value(1000)), new Value(0)), compareList({ 5 }));
    expectConstantUntil(new Repeat(new Hold(new List({ new Value(5) }), new Value(1000)), new Value(1)), compareList({ 5 }), 1000, compareList({}));
    expectConstantUntil(new Repeat(new Hold(new List({ new Value(5) }), new Value(1000)), new Value(5)), compareList({ 5 }), 5000, compareList({}));

    endTest();
}
