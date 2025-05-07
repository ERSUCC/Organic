#include "../include/test.h"

TestInfo::TestInfo(const Path* path)
{
    std::vector<std::string> lines;

    if (!path->readLines(lines))
    {
        throw OrganicFileException("Could not open \"" + path->string() + "\".");
    }

    for (unsigned int i = 0; i < lines.size() && lines[i].substr(0, 2) == "//"; i++)
    {
        data.push_back({});

        char* line = lines[i].data() + 3;

        int length = lines[i].size() - 3;

        while (length > 0)
        {
            unsigned int j = 2;

            while (j < length && (line[j] != ' ' || line[j - 1] != '~' || line[j - 2] != ' '))
            {
                j++;
            }

            if (j < length)
            {
                line[j - 2] = '\0';
            }

            data[i].push_back(line);

            line += j + 1;
            length -= j + 1;
        }
    }
}

const Path* Test::sourcePath(const std::string file) const
{
    return Path::relative(file, ORGANIC_TEST_DIR);
}

Parser::Program* Test::parseSource(const Path* path) const
{
    std::unordered_set<const Path*, Path::Hash, Path::Equals> includedPaths = { path };

    return (new Parser::Parser(path, new Parser::ParserContext(nullptr, "", {}), includedPaths))->parse();
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

    sectionFailures = 0;
}

void Test::endSection()
{
    indents--;

    if (sectionFailures == 0)
    {
        print("[ Suite passed ]");
    }

    else if (sectionFailures == 1)
    {
        print("[ Suite had 1 failing test ]");

        suiteFailures++;
    }

    else
    {
        print("[ Suite had " + std::to_string(sectionFailures) + " failing tests ]");

        suiteFailures++;
    }
}

void Test::beginTest(const std::string name)
{
    print(name);

    indents++;

    testFailures = 0;
}

void Test::endTest()
{
    indents--;

    if (testFailures == 1)
    {
        print("Test failed with " + std::to_string(testFailures) + " incorrect assertion");
    }

    else if (testFailures != 0)
    {
        print("Test failed with " + std::to_string(testFailures) + " incorrect assertions");
    }

    sectionFailures += testFailures;
}

void Test::assert(const std::string name, const bool result)
{
    if (!result)
    {
        print("Failed assertion: " + name);

        testFailures++;
    }
}

void Test::fail(const std::string message)
{
    print("Failed test with message: " + message);

    testFailures++;
}

unsigned int TokenizerTests::test()
{
    suiteFailures = 0;

    beginSection("Token list");

    for (const Path* path : sourcePath("tokenizer/token-list")->children())
    {
        checkList(path);
    }

    endSection();

    return suiteFailures;
}

void TokenizerTests::checkList(const Path* path)
{
    TestInfo* info = new TestInfo(path);

    beginTest(info->data[0][0]);

    std::string source;

    if (path->readToString(source))
    {
        const Parser::TokenList* list = (new Parser::Tokenizer(path))->tokenize();

        Parser::TokenListNode* current = list->head->next;

        for (unsigned int i = 0; i < info->data[1].size(); i++)
        {
            if (current->token->string() != info->data[1][i])
            {
                break;
            }

            current = current->next;
        }

        assert("Tokenized list matches expected list", current->end);
    }

    else
    {
        fail("Could not read file \"" + path->string() + "\".");
    }

    endTest();
}

unsigned int ParserTests::test()
{
    suiteFailures = 0;

    beginSection("Parser round trip");

    for (const Path* path : sourcePath("parser/round-trip")->children())
    {
        roundTrip(path);
    }

    endSection();

    beginSection("Parser errors");

    for (const Path* path : sourcePath("parser/errors")->children())
    {
        expectError(path);
    }

    endSection();

    return suiteFailures;
}

void ParserTests::roundTrip(const Path* path)
{
    TestInfo* info = new TestInfo(path);

    beginTest(info->data[0][0]);

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

void ParserTests::expectError(const Path* path)
{
    TestInfo* info = new TestInfo(path);

    beginTest(info->data[0][0]);

    std::string source;

    if (path->readToString(source))
    {
        bool error = false;

        try
        {
            parseSource(path);

            fail("Parser did not throw any errors.");
        }

        catch (const OrganicException& e)
        {
            const std::vector<std::string>& error = info->data[1];

            assert("Parser throws expected error", e == OrganicParseException(error[2], SourceLocation(path, std::stoi(error[0]), std::stoi(error[1]))));
        }
    }

    else
    {
        fail("Could not read file \"" + path->string() + "\".");
    }

    endTest();
}

std::string ParserTests::formatSource(const std::string text) const
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

unsigned int TypeResolverTests::test()
{
    suiteFailures = 0;

    beginSection("Type inference");

    for (const Path* path : sourcePath("type-resolver/inference")->children())
    {
        expectSuccess(path);
    }

    endSection();

    beginSection("Type resolver errors");

    for (const Path* path : sourcePath("type-resolver/errors")->children())
    {
        expectError(path);
    }

    endSection();

    return suiteFailures;
}

void TypeResolverTests::expectSuccess(const Path* path)
{
    TestInfo* info = new TestInfo(path);

    beginTest(info->data[0][0]);

    std::string source;

    try
    {
        parseSource(path)->resolveTypes(new Parser::TypeResolver(path));
    }

    catch (const OrganicException& e)
    {
        fail("Expected success, received an error.");
    }

    endTest();
}

void TypeResolverTests::expectError(const Path* path)
{
    TestInfo* info = new TestInfo(path);

    beginTest(info->data[0][0]);

    try
    {
        parseSource(path)->resolveTypes(new Parser::TypeResolver(path));

        fail("Type resolver did not throw any errors.");
    }

    catch (const OrganicException& e)
    {
        const std::vector<std::string>& error = info->data[1];

        assert("Type resolver throws expected error", e == OrganicParseException(error[2], SourceLocation(path, std::stoi(error[0]), std::stoi(error[1]))));
    }

    endTest();
}
