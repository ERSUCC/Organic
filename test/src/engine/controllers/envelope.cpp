#include "engine/test_controllers.h"

void TestControllers::testEnvelope()
{
    beginTest("Envelope", true);

    expectConstantUntil(new Envelope(new Value(0), new Value(0), new Value(0), new Value(0), new Value(0)), 0, 0);

    expectConstantUntil(new Envelope(new Value(0), new Value(0), new Value(1000), new Value(0), new Value(0)), 0, 1000);
    expectConstantUntil(new Envelope(new Value(0), new Value(0), new Value(0), new Value(1000), new Value(0)), 0, 1000);
    expectConstantUntil(new Envelope(new Value(0), new Value(0), new Value(0), new Value(0), new Value(1000)), 0, 1000);

    expectConstantUntil(new Envelope(new Value(1), new Value(1), new Value(1000), new Value(0), new Value(0)), 1, 1000);
    expectConstantUntil(new Envelope(new Value(1), new Value(1), new Value(0), new Value(1000), new Value(0)), 1, 1000);
    expectConstantUntil(new Envelope(new Value(1), new Value(1), new Value(0), new Value(0), new Value(1000)), 1, 1000);

    expectConstantUntil(new Envelope(new Value(1), new Value(1), new Value(1000), new Value(1000), new Value(0)), 1, 2000);
    expectConstantUntil(new Envelope(new Value(1), new Value(1), new Value(0), new Value(1000), new Value(1000)), 1, 2000);
    expectConstantUntil(new Envelope(new Value(1), new Value(1), new Value(1000), new Value(0), new Value(1000)), 1, 2000);
    expectConstantUntil(new Envelope(new Value(1), new Value(1), new Value(1000), new Value(1000), new Value(1000)), 1, 3000);

    expectValues(new Envelope(new Value(0), new Value(1), new Value(1000), new Value(0), new Value(0)),
    {
        TimeValue(0, 0),
        TimeValue(250, 0.25),
        TimeValue(500, 0.5),
        TimeValue(750, 0.75),
        TimeValue(1000, 0)
    });

    expectValues(new Envelope(new Value(0), new Value(1), new Value(0), new Value(1000), new Value(0)),
    {
        TimeValue(0, 1),
        TimeValue(250, 1),
        TimeValue(500, 1),
        TimeValue(750, 1),
        TimeValue(1000, 0)
    });

    expectValues(new Envelope(new Value(0), new Value(1), new Value(0), new Value(0), new Value(1000)),
    {
        TimeValue(0, 1),
        TimeValue(250, 0.75),
        TimeValue(500, 0.5),
        TimeValue(750, 0.25),
        TimeValue(1000, 0)
    });

    expectValues(new Envelope(new Value(0), new Value(1), new Value(1000), new Value(0), new Value(1000)),
    {
        TimeValue(0, 0),
        TimeValue(250, 0.25),
        TimeValue(500, 0.5),
        TimeValue(750, 0.75),
        TimeValue(1000, 1),
        TimeValue(1250, 0.75),
        TimeValue(1500, 0.5),
        TimeValue(1750, 0.25),
        TimeValue(2000, 0)
    });

    expectValues(new Envelope(new Value(1), new Value(0), new Value(1000), new Value(0), new Value(1000)),
    {
        TimeValue(0, 1),
        TimeValue(250, 0.75),
        TimeValue(500, 0.5),
        TimeValue(750, 0.25),
        TimeValue(1000, 0),
        TimeValue(1250, 0.25),
        TimeValue(1500, 0.5),
        TimeValue(1750, 0.75),
        TimeValue(2000, 0)
    });

    expectValues(new Envelope(new Value(0.5), new Value(1), new Value(1000), new Value(1000), new Value(1000)),
    {
        TimeValue(0, 0.5),
        TimeValue(250, 0.625),
        TimeValue(500, 0.75),
        TimeValue(750, 0.875),
        TimeValue(1000, 1),
        TimeValue(1250, 1),
        TimeValue(1500, 1),
        TimeValue(1750, 1),
        TimeValue(2000, 1),
        TimeValue(2250, 0.875),
        TimeValue(2500, 0.75),
        TimeValue(2750, 0.625),
        TimeValue(3000, 0),
    });

    expectValues(new Envelope(new Value(0), new Value(-1), new Value(1000), new Value(0), new Value(1000)),
    {
        TimeValue(0, 0),
        TimeValue(250, -0.25),
        TimeValue(500, -0.5),
        TimeValue(750, -0.75),
        TimeValue(1000, -1),
        TimeValue(1250, -0.75),
        TimeValue(1500, -0.5),
        TimeValue(1750, -0.25),
        TimeValue(2000, 0)
    });

    endTest();
}
