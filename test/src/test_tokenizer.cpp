#include "../include/test_tokenizer.h"

TestTokenizer::TestTokenizer(TestTracker* tracker) :
    Test(tracker) {}

void TestTokenizer::test()
{
    beginSuite("Token list");

    for (const Path* path : sourcePath("tokenizer/token-list")->children())
    {
        checkList(path);
    }

    beginSuite("Tokenizer errors");

    for (const Path* path : sourcePath("tokenizer/errors")->children())
    {
        expectError(path);
    }
}

void TestTokenizer::checkList(const Path* path)
{
    const OTest* info = new OTest(path);

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

void TestTokenizer::expectError(const Path* path)
{
    const OTest* info = new OTest(path);

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
