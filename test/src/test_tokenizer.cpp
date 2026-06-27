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
    beginTest(info->getValue("warn")->asBoolean());

    const SourceProvider* source = new SourceProvider(info->getSource());

    Parser::TokenIterator* tokens = Parser::Tokenizer::tokenize(source);

    for (const TOMLValue* token : info->getValue("tokens")->asArray())
    {
        if (tokens->peek()->string() != token->asString())
        {
            break;
        }

        tokens->drop();
    }

    assert("Tokenized list matches expected list", tokens->peek()->eof());

    delete tokens;
    delete source;

    endTest(info->getValue("name")->asString());
}

void TestTokenizer::expectError(const OTest* info)
{
    beginTest(info->getValue("warn")->asBoolean());

    const SourceProvider* source = new SourceProvider(info->getSource());

    try
    {
        delete Parser::Tokenizer::tokenize(source);

        fail("Tokenizer did not throw any errors.");
    }

    catch (const OrganicParseException& e)
    {
        assert("Tokenizer throws expected error", matchParseError(info, e));
    }

    catch (const OrganicException& e)
    {
        fail("Tokenizer did not throw the expected error.");
    }

    delete source;

    endTest(info->getValue("name")->asString());
}
