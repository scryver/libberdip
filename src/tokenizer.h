struct SourcePos
{
    String filename;
    u32 line;
    u32 column;
};

enum TokenKind
{
    Token_None,

    Token_Name,         // a..zA..Z_...
    Token_Integer,      // 12 0x12 0b10 12e10 12e+10 12e-10 etc
    Token_Float,        // 0.12 0.12f 0.12e10 0.12e+10 0.12e-10 etc
    Token_String,       // "..."

    Token_Dot,          // .
    Token_Comma,        // ,
    Token_Colon,        // :
    Token_SemiColon,    // ;
    Token_Newline,

    Token_ParenOpen,    // (
    Token_ParenClose,   // )
    Token_BraceOpen,    // {
    Token_BraceClose,   // }
    Token_BracketOpen,  // [
    Token_BracketClose, // ]

    Token_LogicalOr,    // ||
    Token_LogicalAnd,   // &&

    Token_Eq,           // ==
    Token_Neq,          // !=
    Token_Lt,           // <
    Token_Gt,           // >
    Token_LtEq,         // <=
    Token_GtEq,         // >=

    Token_Or,           // |
    Token_And,          // &
    Token_Xor,          // ^

    Token_Add,          // +
    Token_Subtract,     // -
    Token_Negate = Token_Subtract,

    Token_ShiftLeft,    // <<
    Token_ShiftRight,   // >>

    Token_Multiply,     // *
    Token_Divide,       // /
    Token_Modulus,      // %

    Token_Not,          // !
    Token_Invert,       // ~

    Token_Increment,    // ++
    Token_Decrement,    // --

    Token_Assign,       // =
    Token_OrAssign,     // |=
    Token_AndAssign,    // &=
    Token_XorAssign,    // ^=
    Token_InvAssign,    // ~=
    Token_AddAssign,    // +=
    Token_SubAssign,    // -=
    Token_SlAssign,     // <<=
    Token_SrAssign,     // >>=
    Token_MulAssign,    // *=
    Token_DivAssign,    // /=
    Token_ModAssign,    // %=

    Token_Quote,        // '
    Token_Backslash,    // \ <= Extra space in comment needed otherwise the newline is escaped
    Token_Number,       // #
    Token_Dollar,       // $
    Token_Query,        // ?

    Token_EOF,

    TokenCount
};
struct Token
{
    TokenKind kind;
    SourcePos origin;
    u32 indent;
    String value; // TODO(michiel): Rename to text
    union {
        s32 s32; // TODO(michiel): Implement at get_token level
        u32 u32;
        f32 f32;
    };
};

struct Tokenizer
{
    SourcePos origin;
    String scanner;

    b32 error;
};

internal Token get_token(Tokenizer *tokenizer);
internal Token peek_token(Tokenizer *tokenizer);
internal b32   is_parsing(Tokenizer *tokenizer);
internal b32   is_valid(Token token);
internal b32   match_token(Tokenizer *tokenizer, TokenKind tokenKind);
internal b32   match_token_name(Tokenizer *tokenizer, String name);
internal Token expect_token(Tokenizer *tokenizer, TokenKind tokenKind);
internal Token expect_name(Tokenizer *tokenizer, String name);
internal Token expect_integer_range(Tokenizer *tokenizer, s32 minValue, s32 maxValue);
internal void  print_token(Token token);
