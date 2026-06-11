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
    const TestTokenizerInfo* info = new TestTokenizerInfo(path);

    beginTest(info);

    if (const SourceFile* source = SourceFile::create(path))
    {
        const Parser::TokenList* list = (new Parser::Tokenizer(source))->tokenize();

        Parser::TokenListNode* current = list->head->next;

        for (const std::string& token : info->tokens())
        {
            if (current->token->string() != token)
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
