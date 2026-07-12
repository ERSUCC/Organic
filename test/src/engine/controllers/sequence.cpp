#include "engine/test_controllers.h"

void TestControllers::testSequence()
{
    beginTest("Sequence", true);

    expectConstant(new Sequence(new List({ new Value(0) }), new ValueByte(Constants::Sequence::Forward)), 0);
    expectConstant(new Sequence(new List({ new Value(5) }), new ValueByte(Constants::Sequence::Forward)), 5);
    expectConstant(new Sequence(new List({ new Value(-5) }), new ValueByte(Constants::Sequence::Forward)), -5);
    expectConstantUntil(new Sequence(new List({ new Hold(new Value(5), new Value(1000)) }), new ValueByte(Constants::Sequence::Forward)), 5, 1000);

    expectValues(new Sequence(new List(
    {
        new Hold(new Value(5), new Value(1000)),
        new Hold(new Value(6), new Value(1000)),
        new Hold(new Value(7), new Value(1000))
    }), new ValueByte(Constants::Sequence::Forward)),
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
    }), new ValueByte(Constants::Sequence::Backward)),
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
    }), new ValueByte(Constants::Sequence::PingPong)),
    {
        TimeValue(0, 5),
        TimeValue(1, 5),
        TimeValue(1000, 6),
        TimeValue(1001, 6),
        TimeValue(2000, 7),
        TimeValue(2001, 7),
        TimeValue(3000, 6),
        TimeValue(3001, 6),
        TimeValue(4000, 0)
    });

    expectValues(new Sequence(new List(
    {
        new Hold(new Value(5), new Value(1000)),
        new Hold(new Value(6), new Value(1000)),
        new Hold(new Value(7), new Value(1000))
    }), new ValueByte(Constants::Sequence::Shuffle)),
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
}
