#include "engine/test_controllers.h"

void TestControllers::testTrigger()
{
    beginTest("Trigger (number)", true);

    expectConstant(new Trigger(new Value(0), new Value(0)), 0);
    expectConstant(new Trigger(new Value(0), new Value(5)), 0);
    expectConstant(new Trigger(new Value(1), new Value(0)), 0);
    expectConstant(new Trigger(new Value(1), new Value(5)), 5);

    expectValues(new Trigger(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new Value(5)),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 5),
        TimeValue(750, 5),
        TimeValue(1000, 5)
    });

    expectValues(new Trigger(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new Hold(new Value(5), new Value(1000))),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 5),
        TimeValue(750, 5),
        TimeValue(1000, 5),
        TimeValue(1250, 5),
        TimeValue(1500, 0)
    });

    endTest();

    beginTest("Trigger (constant)", true);

    expectConstant(new Trigger(new Value(0), new ValueChar(0)), compareChar(0));
    expectConstant(new Trigger(new Value(0), new ValueChar(5)), compareChar(0));
    expectConstant(new Trigger(new Value(1), new ValueChar(0)), compareChar(0));
    expectConstant(new Trigger(new Value(1), new ValueChar(5)), compareChar(5));

    expectValues(new Trigger(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new ValueChar(5)),
    {
        TimeLambda(0, compareChar(0)),
        TimeLambda(250, compareChar(0)),
        TimeLambda(500, compareChar(5)),
        TimeLambda(750, compareChar(5)),
        TimeLambda(1000, compareChar(5))
    });

    expectValues(new Trigger(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new Hold(new ValueChar(5), new Value(1000))),
    {
        TimeLambda(0, compareChar(0)),
        TimeLambda(250, compareChar(0)),
        TimeLambda(500, compareChar(5)),
        TimeLambda(750, compareChar(5)),
        TimeLambda(1000, compareChar(5)),
        TimeLambda(1250, compareChar(5)),
        TimeLambda(1500, compareChar(0))
    });

    endTest();

    beginTest("Trigger (list)", true);

    expectConstant(new Trigger(new Value(0), new List({ new Value(0) })), compareList({}));
    expectConstant(new Trigger(new Value(0), new List({ new Value(5) })), compareList({}));
    expectConstant(new Trigger(new Value(1), new List({ new Value(0) })), compareList({ 0 }));
    expectConstant(new Trigger(new Value(1), new List({ new Value(5) })), compareList({ 5 }));

    expectValues(new Trigger(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new List({ new Value(5) })),
    {
        TimeLambda(0, compareList({})),
        TimeLambda(250, compareList({})),
        TimeLambda(500, compareList({ 5 })),
        TimeLambda(750, compareList({ 5 })),
        TimeLambda(1000, compareList({ 5 }))
    });

    expectValues(new Trigger(new ValueGreaterEqual(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(2.5)), new Hold(new List({ new Value(5) }), new Value(1000))),
    {
        TimeLambda(0, compareList({})),
        TimeLambda(250, compareList({})),
        TimeLambda(500, compareList({ 5 })),
        TimeLambda(750, compareList({ 5 })),
        TimeLambda(1000, compareList({ 5 })),
        TimeLambda(1250, compareList({ 5 })),
        TimeLambda(1500, compareList({}))
    });

    endTest();
}
