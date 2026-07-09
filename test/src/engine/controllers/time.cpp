#include "engine/test_controllers.h"

void TestControllers::testTime()
{
    beginTest("Time", true);

    expectValues(new Time(),
    {
        TimeValue(0, 0),
        TimeValue(1, 1),
        TimeValue(1000, 1000)
    });

    endTest();
}
