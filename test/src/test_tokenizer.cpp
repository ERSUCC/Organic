#include "../include/test_tokenizer.h"

TestTokenizer::TestTokenizer(TestTracker* tracker) :
    Test(tracker) {}

void TestTokenizer::test()
{
    beginSuite("Token list");

    for (const Path* path : sourcePath("tokenizer/token-list")->children())
    {
        for (const OTest* info : OTest::read(path))
        {
            checkList(info);
        }
    }

    beginSuite("Tokenizer errors");

    for (const Path* path : sourcePath("tokenizer/errors")->children())
    {
        for (const OTest* info : OTest::read(path))
        {
            expectError(info);
        }
    }
}

void TestTokenizer::checkList(const OTest* info)
{
    beginTest(info);

    const Parser::TokenList* list = (new Parser::Tokenizer(new SourceProvider(info->getSource())))->tokenize();

    Parser::TokenListNode* current = list->head->next;

    for (const TOMLValue* token : info->getValue("tokens")->asArray()->values)
    {
        if (current->token->string() != token->asString()->str)
        {
            break;
        }

        current = current->next;
    }

    assert("Tokenized list matches expected list", current->end);

    endTest();
}

void TestTokenizer::expectError(const OTest* info)
{
    beginTest(info);

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

    endTest();
}
