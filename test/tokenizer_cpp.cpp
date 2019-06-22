#include "../src/tokenizer.h"
#include "../src/tokenizer.cpp"

TEST_BEGIN(tokenizer)
{
    ApiFile testFile = read_entire_file("../../src/tokenizer.cpp");
    i_expect_not_equal(testFile.content.size, 0ULL);

    Tokenizer tokenizer = {0};
    tokenizer.origin.filename = string("tokenizer.cpp");
    tokenizer.scanner = string(testFile.content.size, testFile.content.data);

    Token token = {Token_None};
    do
    {
        token = get_token(&tokenizer);
        print_token(token);

    } while (token.kind != Token_EOF);
}
TEST_END(tokenizer)
