#include <stddef.h>
#include <string>

#include "../include/test_examples.h"
#include "../include/test_parser.h"
#include "../include/test_resolver.h"
#include "../include/test_tokenizer.h"
#include "../include/test_utils.h"
#include "../include/engine/test_controllers.h"

int main(int argc, char** argv)
{
    Utils* utils = Utils::get();

    utils->channels = 2;
    utils->sampleRate = 44100;
    utils->timeStep = 1000.0 / utils->sampleRate;

    TestTracker* tracker = new TestTracker();

    tracker->beginSection();

    try
    {
        TestTokenizer::run(tracker);
        TestParser::run(tracker);
        TestResolver::run(tracker);
        TestControllers::run(tracker);
        TestExamples::run(tracker);
    }

    catch (const OrganicException& e)
    {
        TestUtils::printError(e.what());

        delete utils;

        return 1;
    }

    const size_t failures = tracker->endSection();

    delete tracker;

    if (failures == 0)
    {
        TestUtils::printSuccess("\nAll tests passed.");

        delete utils;

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

    delete utils;

    return 1;
}
