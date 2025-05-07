#include <iostream>

#include "../include/test.h"

int main(int argc, char** argv)
{
    unsigned int suiteFailures = 0;

    suiteFailures += (new TokenizerTests())->test();
    suiteFailures += (new ParserTests())->test();
    suiteFailures += (new TypeResolverTests())->test();

    std::cout << "\n~~~ Test Summary ~~~\n\n";

    if (suiteFailures == 0)
    {
        std::cout << "All tests passed.\n";
    }

    else if (suiteFailures == 1)
    {
        std::cout << "1 failing suite.\n";
    }

    else if (suiteFailures)
    {
        std::cout << std::to_string(suiteFailures) + " failing suites.\n";
    }

    return 0;
}
