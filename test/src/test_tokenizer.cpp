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
