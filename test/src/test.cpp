#include "../include/test.h"

void Test::testAll()
{
    (new ParserTest())->test();
}

const Path* Test::sourcePath(const std::string file) const
{
    return Path::relative(file, ORGANIC_TEST_DIR);
}

const Parser::Program* Test::parseSource(const Path* path) const
{
    return (new Parser::Parser(path, std::unordered_set<const Path*, Path::Hash, Path::Equals> { path }))->parse();
}

void Test::print(const std::string text)
{
    for (unsigned int i = 0; i < indents; i++)
    {
        std::cout << '\t';
    }

    std::cout << text << "\n";
}

void Test::beginSection(const std::string name)
{
    print("[ " + name + " ]");

    indents++;
}

void Test::endSection()
{
    indents--;
}

void Test::beginTest(const std::string name)
{
    print(name);

    indents++;

    failures = 0;
}

void Test::endTest()
{
    indents--;

    if (failures == 0)
    {
        print("Test passed");
    }

    else
    {
        if (failures == 1)
        {
            print("Test failed with " + std::to_string(failures) + " incorrect assertion");
        }

        else
        {
            print("Test failed with " + std::to_string(failures) + " incorrect assertions");
        }
    }
}

void Test::assert(const std::string name, const bool result)
{
    if (!result)
    {
        print("Failed assertion: \"" + name + "\"");

        failures++;
    }
}

void Test::fail(const std::string message)
{
    print("Failed test with message: " + message);

    failures++;
}

void ParserTest::test()
{
    beginSection("Parser round trip");

    roundTrip("Empty file", sourcePath("empty.organic"));
    roundTrip("Empty file with comments", sourcePath("empty-with-comments.organic"));
    roundTrip("Hello world", sourcePath("hello-world.organic"));
    roundTrip("Terrible whitespace", sourcePath("terrible-whitespace.organic"));
    roundTrip("Constants", sourcePath("constants.organic"));
    roundTrip("Simple variable reference", sourcePath("simple-variable.organic"));
    roundTrip("Single nest", sourcePath("single-nest.organic"));
    roundTrip("Multiple nests", sourcePath("multi-nest.organic"));
    roundTrip("Basic list", sourcePath("basic-list.organic"));

    endSection();
}

void ParserTest::roundTrip(const std::string name, const Path* path)
{
    beginTest(name);

    std::string source;

    if (path->readToString(source))
    {
        const Parser::Program* program = parseSource(path);

        assert("Source code is semantically equivalent after round trip", formatSource(source) == formatSource(program->string()));
    }

    else
    {
        fail("Could not read file \"" + path->string() + "\".");
    }

    endTest();
}

std::string ParserTest::formatSource(const std::string text) const
{
    const std::string untrimmed = std::regex_replace(text, std::regex("(//[^\n]*)|(/\\*[\\w\\W]*\\*/)|(\\s+)"), "");

    const size_t first = untrimmed.find_first_not_of(' ');
    const size_t last = untrimmed.find_last_not_of(' ');

    if (first == std::string::npos)
    {
        return "";
    }

    return untrimmed.substr(first, last - first + 1);
}
