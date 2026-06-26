#include "../include/test_tokenizer.h"

TestTokenizer::TestTokenizer(TestTracker* tracker) :
    Test(tracker) {}

void TestTokenizer::test()
{
    beginSuite("Token list");

    for (const Path* path : testPath("tokenizer/token-list")->children())
    {
        for (const OTest* info : OTest::read(path))
        {
            checkList(info);
        }
    }

    beginSuite("Tokenizer errors");

    for (const Path* path : testPath("tokenizer/errors")->children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectError(info);
        }
    }
}

void TestTokenizer::checkList(const OTest* info)
{
    beginTest(info->getValue("warn")->asBoolean()->value);

    Parser::TokenIterator* tokens = (new Parser::Tokenizer(new SourceProvider(info->getSource())))->tokenize();

    for (const TOMLValue* token : info->getValue("tokens")->asArray()->values)
    {
        if (tokens->take()->string() != token->asString()->str)
        {
            break;
        }
    }

    assert("Tokenized list matches expected list", tokens->peek()->eof());

    endTest(info->getValue("name")->asString()->str);
}

void TestTokenizer::expectError(const OTest* info)
{
    beginTest(info->getValue("warn")->asBoolean()->value);

    try
    {
        (new Parser::Tokenizer(new SourceProvider(info->getSource())))->tokenize();

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

    endTest(info->getValue("name")->asString()->str);
}
