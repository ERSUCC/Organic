#include "engine/test_controllers.h"

void TestControllers::testRound()
{
    beginTest("Round", true);

    expectConstant(new Round(new Value(0), new Value(0), new ValueByte(Constants::Round::Nearest)), 0);
    expectConstant(new Round(new Value(0.12345), new Value(0), new ValueByte(Constants::Round::Nearest)), 0.12345);
    expectConstant(new Round(new Value(0.98765), new Value(0), new ValueByte(Constants::Round::Nearest)), 0.98765);
    expectConstant(new Round(new Value(0), new Value(1), new ValueByte(Constants::Round::Nearest)), 0);
    expectConstant(new Round(new Value(0.12345), new Value(1), new ValueByte(Constants::Round::Nearest)), 0);
    expectConstant(new Round(new Value(0.98765), new Value(1), new ValueByte(Constants::Round::Nearest)), 1);
    expectConstant(new Round(new Value(5), new Value(1), new ValueByte(Constants::Round::Nearest)), 5);

    expectValues(new Round(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(1), new ValueByte(Constants::Round::Nearest)),
    {
        TimeValue(0, 0),
        TimeValue(250, 1),
        TimeValue(500, 3),
        TimeValue(750, 4),
        TimeValue(999, 5),
        TimeValue(1000, 0)
    });

    expectValues(new Round(new Sweep(new Value(0), new Value(-5), new Value(1000)), new Value(1), new ValueByte(Constants::Round::Nearest)),
    {
        TimeValue(0, 0),
        TimeValue(250, -1),
        TimeValue(500, -3),
        TimeValue(750, -4),
        TimeValue(999, -5),
        TimeValue(1000, 0)
    });

    expectValues(new Round(new Sweep(new Value(0), new Value(8), new Value(1000)), new Value(3), new ValueByte(Constants::Round::Nearest)),
    {
        TimeValue(0, 0),
        TimeValue(250, 3),
        TimeValue(500, 3),
        TimeValue(750, 6),
        TimeValue(999, 9),
        TimeValue(1000, 0)
    });

    expectConstant(new Round(new Value(0), new Value(0), new ValueByte(Constants::Round::Up)), 0);
    expectConstant(new Round(new Value(0.12345), new Value(0), new ValueByte(Constants::Round::Up)), 0.12345);
    expectConstant(new Round(new Value(0.98765), new Value(0), new ValueByte(Constants::Round::Up)), 0.98765);
    expectConstant(new Round(new Value(0), new Value(1), new ValueByte(Constants::Round::Up)), 0);
    expectConstant(new Round(new Value(0.12345), new Value(1), new ValueByte(Constants::Round::Up)), 1);
    expectConstant(new Round(new Value(0.98765), new Value(1), new ValueByte(Constants::Round::Up)), 1);
    expectConstant(new Round(new Value(5), new Value(1), new ValueByte(Constants::Round::Up)), 5);

    expectValues(new Round(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(1), new ValueByte(Constants::Round::Up)),
    {
        TimeValue(0, 0),
        TimeValue(250, 2),
        TimeValue(500, 3),
        TimeValue(750, 4),
        TimeValue(999, 5),
        TimeValue(1000, 0)
    });

    expectValues(new Round(new Sweep(new Value(0), new Value(-5), new Value(1000)), new Value(1), new ValueByte(Constants::Round::Up)),
    {
        TimeValue(0, 0),
        TimeValue(250, -1),
        TimeValue(500, -2),
        TimeValue(750, -3),
        TimeValue(999, -4),
        TimeValue(1000, 0)
    });

    expectValues(new Round(new Sweep(new Value(0), new Value(8), new Value(1000)), new Value(3), new ValueByte(Constants::Round::Up)),
    {
        TimeValue(0, 0),
        TimeValue(250, 3),
        TimeValue(500, 6),
        TimeValue(750, 6),
        TimeValue(999, 9),
        TimeValue(1000, 0)
    });

    expectConstant(new Round(new Value(0), new Value(0), new ValueByte(Constants::Round::Down)), 0);
    expectConstant(new Round(new Value(0.12345), new Value(0), new ValueByte(Constants::Round::Down)), 0.12345);
    expectConstant(new Round(new Value(0.98765), new Value(0), new ValueByte(Constants::Round::Down)), 0.98765);
    expectConstant(new Round(new Value(0), new Value(1), new ValueByte(Constants::Round::Down)), 0);
    expectConstant(new Round(new Value(0.12345), new Value(1), new ValueByte(Constants::Round::Down)), 0);
    expectConstant(new Round(new Value(0.98765), new Value(1), new ValueByte(Constants::Round::Down)), 0);
    expectConstant(new Round(new Value(5), new Value(1), new ValueByte(Constants::Round::Down)), 5);

    expectValues(new Round(new Sweep(new Value(0), new Value(5), new Value(1000)), new Value(1), new ValueByte(Constants::Round::Down)),
    {
        TimeValue(0, 0),
        TimeValue(250, 1),
        TimeValue(500, 2),
        TimeValue(750, 3),
        TimeValue(999, 4),
        TimeValue(1000, 0)
    });

    expectValues(new Round(new Sweep(new Value(0), new Value(-5), new Value(1000)), new Value(1), new ValueByte(Constants::Round::Down)),
    {
        TimeValue(0, 0),
        TimeValue(250, -2),
        TimeValue(500, -3),
        TimeValue(750, -4),
        TimeValue(999, -5),
        TimeValue(1000, 0)
    });

    expectValues(new Round(new Sweep(new Value(0), new Value(8), new Value(1000)), new Value(3), new ValueByte(Constants::Round::Down)),
    {
        TimeValue(0, 0),
        TimeValue(250, 0),
        TimeValue(500, 3),
        TimeValue(750, 6),
        TimeValue(999, 6),
        TimeValue(1000, 0)
    });

    endTest();
}
