#include "engine/test_controllers.h"

void TestControllers::testSequence()
{
    beginTest("Sequence (number)", true);

    expectConstant(new Sequence(new List({ new Value(0) }), new ValueChar(Constants::Sequence::Forward)), 0);
    expectConstant(new Sequence(new List({ new Value(5) }), new ValueChar(Constants::Sequence::Forward)), 5);
    expectConstant(new Sequence(new List({ new Value(-5) }), new ValueChar(Constants::Sequence::Forward)), -5);
    expectConstantUntil(new Sequence(new List({ new Hold(new Value(5), new Value(1000)) }), new ValueChar(Constants::Sequence::Forward)), 5, 1000);

    expectValues(new Sequence(new List(
    {
        new Hold(new Value(5), new Value(1000)),
        new Hold(new Value(6), new Value(1000)),
        new Hold(new Value(7), new Value(1000))
    }), new ValueChar(Constants::Sequence::Forward)),
    {
        TimeValue(0, 5),
        TimeValue(1, 5),
        TimeValue(1000, 6),
        TimeValue(1001, 6),
        TimeValue(2000, 7),
        TimeValue(2001, 7),
        TimeValue(3000, 0)
    });

    expectValues(new Sequence(new List(
    {
        new Hold(new Value(5), new Value(1000)),
        new Hold(new Value(6), new Value(1000)),
        new Hold(new Value(7), new Value(1000))
    }), new ValueChar(Constants::Sequence::Backward)),
    {
        TimeValue(0, 7),
        TimeValue(1, 7),
        TimeValue(1000, 6),
        TimeValue(1001, 6),
        TimeValue(2000, 5),
        TimeValue(2001, 5),
        TimeValue(3000, 0)
    });

    expectValues(new Sequence(new List(
    {
        new Hold(new Value(5), new Value(1000)),
        new Hold(new Value(6), new Value(1000)),
        new Hold(new Value(7), new Value(1000))
    }), new ValueChar(Constants::Sequence::Shuffle)),
    {
        TimeValue(0, 5),
        TimeValue(1, 5),
        TimeValue(1000, 7),
        TimeValue(1001, 7),
        TimeValue(2000, 6),
        TimeValue(2001, 6),
        TimeValue(3000, 0)
    });

    endTest();

    beginTest("Sequence (constant)", true);

    expectConstant(new Sequence(new List({ new ValueChar(0) }), new ValueChar(Constants::Sequence::Forward)), compareChar(0));
    expectConstant(new Sequence(new List({ new ValueChar(5) }), new ValueChar(Constants::Sequence::Forward)), compareChar(5));
    expectConstantUntil(new Sequence(new List({ new Hold(new ValueChar(5), new Value(1000)) }), new ValueChar(Constants::Sequence::Forward)), compareChar(5), 1000, compareChar(0));

    expectValues(new Sequence(new List(
    {
        new Hold(new ValueChar(5), new Value(1000)),
        new Hold(new ValueChar(6), new Value(1000)),
        new Hold(new ValueChar(7), new Value(1000))
    }), new ValueChar(Constants::Sequence::Forward)),
    {
        TimeLambda(0, compareChar(5)),
        TimeLambda(1, compareChar(5)),
        TimeLambda(1000, compareChar(6)),
        TimeLambda(1001, compareChar(6)),
        TimeLambda(2000, compareChar(7)),
        TimeLambda(2001, compareChar(7)),
        TimeLambda(3000, compareChar(0))
    });

    expectValues(new Sequence(new List(
    {
        new Hold(new ValueChar(5), new Value(1000)),
        new Hold(new ValueChar(6), new Value(1000)),
        new Hold(new ValueChar(7), new Value(1000))
    }), new ValueChar(Constants::Sequence::Backward)),
    {
        TimeLambda(0, compareChar(7)),
        TimeLambda(1, compareChar(7)),
        TimeLambda(1000, compareChar(6)),
        TimeLambda(1001, compareChar(6)),
        TimeLambda(2000, compareChar(5)),
        TimeLambda(2001, compareChar(5)),
        TimeLambda(3000, compareChar(0))
    });

    expectValues(new Sequence(new List(
    {
        new Hold(new ValueChar(5), new Value(1000)),
        new Hold(new ValueChar(6), new Value(1000)),
        new Hold(new ValueChar(7), new Value(1000))
    }), new ValueChar(Constants::Sequence::Shuffle)),
    {
        TimeLambda(0, compareChar(5)),
        TimeLambda(1, compareChar(5)),
        TimeLambda(1000, compareChar(7)),
        TimeLambda(1001, compareChar(7)),
        TimeLambda(2000, compareChar(6)),
        TimeLambda(2001, compareChar(6)),
        TimeLambda(3000, compareChar(0))
    });

    endTest();

    beginTest("Sequence (list)", true);

    expectConstant(new Sequence(new List({ new List({ new Value(0) }) }), new ValueChar(Constants::Sequence::Forward)), compareList({ 0 }));
    expectConstant(new Sequence(new List({ new List({ new Value(5) }) }), new ValueChar(Constants::Sequence::Forward)), compareList({ 5 }));
    expectConstantUntil(new Sequence(new List({ new Hold(new List({ new Value(5) }), new Value(1000)) }), new ValueChar(Constants::Sequence::Forward)), compareList({ 5 }), 1000, compareChar({}));

    expectValues(new Sequence(new List(
    {
        new Hold(new List({ new Value(5) }), new Value(1000)),
        new Hold(new List({ new Value(6) }), new Value(1000)),
        new Hold(new List({ new Value(7) }), new Value(1000))
    }), new ValueChar(Constants::Sequence::Forward)),
    {
        TimeLambda(0, compareList({ 5 })),
        TimeLambda(1, compareList({ 5 })),
        TimeLambda(1000, compareList({ 6 })),
        TimeLambda(1001, compareList({ 6 })),
        TimeLambda(2000, compareList({ 7 })),
        TimeLambda(2001, compareList({ 7 })),
        TimeLambda(3000, compareList({}))
    });

    expectValues(new Sequence(new List(
    {
        new Hold(new List({ new Value(5) }), new Value(1000)),
        new Hold(new List({ new Value(6) }), new Value(1000)),
        new Hold(new List({ new Value(7) }), new Value(1000))
    }), new ValueChar(Constants::Sequence::Backward)),
    {
        TimeLambda(0, compareList({ 7 })),
        TimeLambda(1, compareList({ 7 })),
        TimeLambda(1000, compareList({ 6 })),
        TimeLambda(1001, compareList({ 6 })),
        TimeLambda(2000, compareList({ 5 })),
        TimeLambda(2001, compareList({ 5 })),
        TimeLambda(3000, compareList({}))
    });

    expectValues(new Sequence(new List(
    {
        new Hold(new List({ new Value(5) }), new Value(1000)),
        new Hold(new List({ new Value(6) }), new Value(1000)),
        new Hold(new List({ new Value(7) }), new Value(1000))
    }), new ValueChar(Constants::Sequence::Shuffle)),
    {
        TimeLambda(0, compareList({ 5 })),
        TimeLambda(1, compareList({ 5 })),
        TimeLambda(1000, compareList({ 7 })),
        TimeLambda(1001, compareList({ 7 })),
        TimeLambda(2000, compareList({ 6 })),
        TimeLambda(2001, compareList({ 6 })),
        TimeLambda(3000, compareList({}))
    });

    endTest();
}
