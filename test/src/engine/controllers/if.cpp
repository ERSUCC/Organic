#include "engine/test_controllers.h"

void TestControllers::testIf()
{
    beginTest("If (number)", true);

    expectConstant(new If(new Value(0), new Value(0), new Value(0)), 0);
    expectConstant(new If(new Value(0), new Value(5), new Value(0)), 0);
    expectConstant(new If(new Value(1), new Value(0), new Value(0)), 0);
    expectConstant(new If(new Value(1), new Value(5), new Value(0)), 5);

    expectValues(new If(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new Value(5), new Value(0)),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 5),
        TimeValue(750, 5),
        TimeValue(1000, 0)
    });

    endTest();

    beginTest("If (constant)", true);

    expectConstant(new If(new Value(0), new ValueChar(0), new ValueChar(0)), compareChar(0));
    expectConstant(new If(new Value(0), new ValueChar(5), new ValueChar(0)), compareChar(0));
    expectConstant(new If(new Value(1), new ValueChar(0), new ValueChar(0)), compareChar(0));
    expectConstant(new If(new Value(1), new ValueChar(5), new ValueChar(0)), compareChar(5));

    expectValues(new If(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new ValueChar(5), new ValueChar(0)),
    {
        TimeLambda(0, compareChar(0)),
        TimeLambda(250, compareChar(0)),
        TimeLambda(500, compareChar(5)),
        TimeLambda(750, compareChar(5)),
        TimeLambda(1000, compareChar(0))
    });

    endTest();

    beginTest("If (list)", true);

    expectConstant(new If(new Value(0), new List({ new Value(0) }), new List({ new Value(0) })), compareList({ 0 }));
    expectConstant(new If(new Value(0), new List({ new Value(5) }), new List({ new Value(0) })), compareList({ 0 }));
    expectConstant(new If(new Value(1), new List({ new Value(0) }), new List({ new Value(0) })), compareList({ 0 }));
    expectConstant(new If(new Value(1), new List({ new Value(5) }), new List({ new Value(0) })), compareList({ 5 }));

    expectValues(new If(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new List({ new Value(5) }), new List({ new Value(0) })),
    {
        TimeLambda(0, compareList({ 0 })),
        TimeLambda(250, compareList({ 0 })),
        TimeLambda(500, compareList({ 5 })),
        TimeLambda(750, compareList({ 5 })),
        TimeLambda(1000, compareList({}))
    });

    endTest();
}
