#include <string>

#include "../include/test_parser.h"
#include "../include/test_resolver.h"
#include "../include/test_tokenizer.h"
#include "../include/test_utils.h"

int main(int argc, char** argv)
{
    TestTracker* tracker = new TestTracker();

    tracker->beginSection();

    (new TestTokenizer(tracker))->test();
    (new TestParser(tracker))->test();
    (new TestResolver(tracker))->test();

    const size_t failures = tracker->endSection();

    if (failures == 0)
    {
        TestUtils::printSuccess("\nAll tests passed.");

        return 0;
    }

    if (failures == 1)
    {
        TestUtils::printError("\n1 failing test.");
    }

    else
    {
        TestUtils::printError("\n" + std::to_string(failures) + " failing tests.");
    }

    return 1;
}
