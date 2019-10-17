#include "../src/tokenizer.h"
#include "../src/tokenizer.cpp"

TEST_BEGIN(tokenizer)
{
    Buffer testFile = read_entire_file(string("../../src/tokenizer.cpp"));
    i_expect_not_equal(testFile.size, 0ULL);

    Tokenizer tokenizer = {0};
    tokenizer.origin.filename = string("tokenizer.cpp");
    tokenizer.scanner = testFile;

    Token token = {Token_None};
    do
    {
        token = get_token(&tokenizer);
        if (token.kind == Token_Quote)
        {
            advance_scanner(&tokenizer);
        }
        //print_token(token);

    } while (token.kind != Token_EOF);
}
TEST_END(tokenizer)
