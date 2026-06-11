#include <iostream>

#include "../include/test.h"
#include "../include/test_parser.h"
#include "../include/test_resolver.h"
#include "../include/test_tokenizer.h"

int main(int argc, char** argv)
{
    TestTracker* tracker = new TestTracker();

    tracker->beginSection();

    (new TestTokenizer(tracker))->test();
    (new TestParser(tracker))->test();
    (new TestResolver(tracker))->test();

    const size_t failures = tracker->endSection();

    std::cout << "\n~~~ Test Summary ~~~\n\n";

    if (failures == 0)
    {
        std::cout << "All tests passed.\n";
    }

    else if (failures == 1)
    {
        std::cout << "1 failing suite.\n";
    }

    else if (failures)
    {
        std::cout << failures << " failing suites.\n";
    }

    return 0;
}
