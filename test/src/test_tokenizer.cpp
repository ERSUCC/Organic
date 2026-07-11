#include "../include/test_tokenizer.h"

void TestTokenizer::run(TestTracker* tracker)
{
    TestTokenizer* test = new TestTokenizer(tracker);

    test->test();

    delete test;
}

void TestTokenizer::test()
{
    beginSuite("Token list");

    for (const Path& path : testPath("tokenizer/token-list").children())
    {
        for (const OTest* info : OTest::read(path))
        {
            checkList(info);

            delete info;
        }
    }

    beginSuite("Tokenizer errors");

    for (const Path& path : testPath("tokenizer/errors").children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectError(info);

            delete info;
        }
    }
}

TestTokenizer::TestTokenizer(TestTracker* tracker) :
    Test(tracker) {}

void TestTokenizer::checkList(const OTest* info)
{
    beginTest(info);

    const NamedSourceProvider* source = new NamedSourceProvider(info->path(), info->getSource());

    Parser::TokenIterator* tokens = nullptr;

    try
    {
        tokens = Parser::Tokenizer::tokenize(source);

        for (const TOMLValue* token : info->getValue("tokens")->asArray())
        {
            if (tokens->peek()->string() != token->asString())
            {
                break;
            }

            tokens->drop();
        }
    }

    catch (const OrganicException& e)
    {
        failWithError(e);
    }

    if (!tokens->peek()->eof())
    {
        fail("Unexpected token in tokenized list: '" + tokens->peek()->string() + "'");
    }

    delete tokens;
    delete source;

    endTest();
}

void TestTokenizer::expectError(const OTest* info)
{
    beginTest(info);

    const NamedSourceProvider* source = new NamedSourceProvider(info->path(), info->getSource());

    try
    {
        delete Parser::Tokenizer::tokenize(source);

        fail("Expected error, but no error was thrown.");
    }

    catch (const OrganicParseException& e)
    {
        expectParseError(info, e);
    }

    catch (const OrganicException& e)
    {
        failAndCompare(info, e);
    }

    delete source;

    endTest();
}
