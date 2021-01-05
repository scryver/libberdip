internal String
get_token_kind_name(TokenKind token)
{
    String result = static_string("unknown");
    switch (token)
    {
        case Token_None          : { result = static_string("NONE"); } break;
        case Token_Name          : { result = static_string("name"); } break;
        case Token_Integer       : { result = static_string("integer"); } break;
        case Token_Float         : { result = static_string("float"); } break;
        case Token_String        : { result = static_string("string"); } break;
        case Token_Dot           : { result = static_string("dot"); } break;
        case Token_Comma         : { result = static_string("comma"); } break;
        case Token_Colon         : { result = static_string("colon"); } break;
        case Token_SemiColon     : { result = static_string("semicolon"); } break;
        case Token_Newline       : { result = static_string("new line"); } break;
        case Token_ParenOpen     : { result = static_string("opening parenthesis"); } break;
        case Token_ParenClose    : { result = static_string("closing parenthesis"); } break;
        case Token_BraceOpen     : { result = static_string("opening brace"); } break;
        case Token_BraceClose    : { result = static_string("closing brace"); } break;
        case Token_BracketOpen   : { result = static_string("opening bracket"); } break;
        case Token_BracketClose  : { result = static_string("closing bracket"); } break;
        case Token_LogicalOr     : { result = static_string("logical or"); } break;
        case Token_LogicalAnd    : { result = static_string("logical and"); } break;
        case Token_Eq            : { result = static_string("equal"); } break;
        case Token_Neq           : { result = static_string("not equal"); } break;
        case Token_Lt            : { result = static_string("less than"); } break;
        case Token_Gt            : { result = static_string("greater than"); } break;
        case Token_LtEq          : { result = static_string("equal or less than"); } break;
        case Token_GtEq          : { result = static_string("equal or greater than"); } break;
        case Token_Or            : { result = static_string("or"); } break;
        case Token_And           : { result = static_string("and"); } break;
        case Token_Xor           : { result = static_string("xor"); } break;
        case Token_Add           : { result = static_string("add"); } break;
        case Token_Subtract      : { result = static_string("subtract"); } break;
        //case Token_Negate        : { result = static_string("negate"); } break;
        case Token_ShiftLeft     : { result = static_string("shift left"); } break;
        case Token_ShiftRight    : { result = static_string("shift right"); } break;
        case Token_Multiply      : { result = static_string("multiply"); } break;
        case Token_Divide        : { result = static_string("divide"); } break;
        case Token_Modulus       : { result = static_string("modulus"); } break;
        case Token_Not           : { result = static_string("not"); } break;
        case Token_Invert        : { result = static_string("invert"); } break;
        case Token_Increment     : { result = static_string("increment"); } break;
        case Token_Decrement     : { result = static_string("decrement"); } break;
        case Token_Assign        : { result = static_string("assign"); } break;
        case Token_OrAssign      : { result = static_string("or assign"); } break;
        case Token_AndAssign     : { result = static_string("and assign"); } break;
        case Token_XorAssign     : { result = static_string("xor assign"); } break;
        case Token_AddAssign     : { result = static_string("add assign"); } break;
        case Token_SubAssign     : { result = static_string("sub assign"); } break;
        case Token_SlAssign      : { result = static_string("shift left assign"); } break;
        case Token_SrAssign      : { result = static_string("shift right assign"); } break;
        case Token_MulAssign     : { result = static_string("multiply assign"); } break;
        case Token_DivAssign     : { result = static_string("divide assign"); } break;
        case Token_ModAssign     : { result = static_string("modulo assign"); } break;
        case Token_Quote         : { result = static_string("quote"); } break;
        case Token_Backslash     : { result = static_string("backslash"); } break;
        case Token_Number        : { result = static_string("number sign"); } break;
        case Token_Dollar        : { result = static_string("dollar sign"); } break;
        case Token_Query         : { result = static_string("question mark"); } break;
        case Token_EOF           : { result = static_string("end of file"); } break;
        INVALID_DEFAULT_CASE;
    }
    return result;
}

// TODO(michiel): Add Token to error
internal void
tokenize_error(Tokenizer *tokenizer, char *fmt, va_list args)
{
#if !COMPILER_MSVC
    fprintf(stderr, "TOKENIZER::ERROR::%.*s:%d:%d\n\t", STR_FMT(tokenizer->origin.filename),
            tokenizer->origin.line,
            tokenizer->origin.column);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
#endif

    tokenizer->error = true;
}

internal void
tokenize_error(Tokenizer *tokenizer, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    tokenize_error(tokenizer, fmt, args);
    va_end(args);
}

internal void
advance_scanner(Tokenizer *tokenizer)
{
    if ((tokenizer->scanner.data[0] == '\n') ||
        (tokenizer->scanner.data[0] == '\r'))
    {
        ++tokenizer->origin.line;
        tokenizer->origin.column = 1;
    }
    else
    {
        ++tokenizer->origin.column;
    }

    ++tokenizer->scanner.data;
    --tokenizer->scanner.size;
}

internal b32
is_valid(Tokenizer *tokenizer)
{
    return !tokenizer->error;
}

#define CASE1(t, k) \
case t: { \
    result.kind = k; \
    result.value = string(1, tokenizer->scanner.data); \
    advance_scanner(tokenizer); \
} break

#define CASE2(t, k1, t2, k2) \
case t: { \
    result.kind = k1; \
    result.value = string(1, tokenizer->scanner.data); \
    advance_scanner(tokenizer); \
    if (tokenizer->scanner.data[0] == t2) { \
        result.kind = k2; \
        ++result.value.size; \
        advance_scanner(tokenizer); \
    } \
} break

#define CASE2B(t, k1, t2a, k2a, t2b, k2b) \
case t: { \
    result.kind = k1; \
    result.value = string(1, tokenizer->scanner.data); \
    advance_scanner(tokenizer); \
    if (tokenizer->scanner.data[0] == t2a) { \
        result.kind = k2a; \
        ++result.value.size; \
        advance_scanner(tokenizer); \
    } else if (tokenizer->scanner.data[0] == t2b) { \
        result.kind = k2b; \
        ++result.value.size; \
        advance_scanner(tokenizer); \
    } \
} break

#define CASE3B(t, k1, t2a, k2a, t2b, k2b, t3, k3) \
case t: { \
    result.kind = k1; \
    result.value = string(1, tokenizer->scanner.data); \
    advance_scanner(tokenizer); \
    if (tokenizer->scanner.data[0] == t2a) { \
        result.kind = k2a; \
        ++result.value.size; \
        advance_scanner(tokenizer); \
    } else if (tokenizer->scanner.data[0] == t2b) { \
        result.kind = k2b; \
        ++result.value.size; \
        advance_scanner(tokenizer); \
        if (tokenizer->scanner.data[0] == t3) { \
            result.kind = k3; \
            ++result.value.size; \
            advance_scanner(tokenizer); \
        } \
    } \
} break


internal Token
get_token(Tokenizer *tokenizer)
{
    Token result = {Token_None};

    repeat:
    result.origin = tokenizer->origin;

    if (!tokenizer->scanner.data[0] ||
        (tokenizer->scanner.size == 0)
#if !COMPILER_MSVC
        // NOTE(michiel): Bah..., maybe just replace the EOF on the platform side
        || ((char)tokenizer->scanner.data[0] == EOF)
#endif
        ) {
        result.kind = Token_EOF;
        return result;
    }

    switch (tokenizer->scanner.data[0])
    {
        case ' ': {
            ++result.indent;
            advance_scanner(tokenizer);
            goto repeat;
        } break;

        case '\r':
        case '\n': {
            u8 prev = tokenizer->scanner.data[0];

            result.kind = Token_Newline;
            result.value = string(1, "\n");
            advance_scanner(tokenizer);
            if (((prev == '\r') && (tokenizer->scanner.data[0] == '\n')) ||
                ((prev == '\n') && (tokenizer->scanner.data[0] == '\r')))
            {
                ++tokenizer->scanner.data;
                --tokenizer->scanner.size;
            }
        } break;

        case '\t':
        case '\v': {
            result.indent += 4; // TODO(michiel): Maybe only for \t?
            advance_scanner(tokenizer);
            goto repeat;
            // tokenize_error(tokenizer, "Only spaces are supported for alignment, no tabs please.");
        } break;

        CASE1('.',  Token_Dot);
        CASE1(',',  Token_Comma);
        CASE1(':',  Token_Colon);
        CASE1(';',  Token_SemiColon);
        CASE1('(',  Token_ParenOpen);
        CASE1(')',  Token_ParenClose);
        CASE1('{',  Token_BraceOpen);
        CASE1('}',  Token_BraceClose);
        CASE1('[',  Token_BracketOpen);
        CASE1(']',  Token_BracketClose);
        CASE1('\'', Token_Quote);
        CASE1('\\', Token_Backslash);
        CASE1('#',  Token_Number);
        CASE1('$',  Token_Dollar);
        CASE1('?',  Token_Query);

        CASE2B('|', Token_Or, '|', Token_LogicalOr, '=', Token_OrAssign);
        CASE2B('&', Token_And, '&', Token_LogicalAnd, '=', Token_AndAssign);
        CASE2('^',  Token_Xor, '=', Token_XorAssign);
        CASE2('~',  Token_Invert, '=', Token_InvAssign);

        CASE2('=',  Token_Assign, '=', Token_Eq);
        CASE2('!',  Token_Not, '=', Token_Neq);

        CASE2B('+', Token_Add, '+', Token_Increment, '=', Token_AddAssign);
        CASE2B('-', Token_Subtract, '-', Token_Decrement, '=', Token_SubAssign);
        CASE2('*',  Token_Multiply, '=', Token_MulAssign);
        CASE2('%',  Token_Modulus, '=', Token_ModAssign);

        CASE3B('<', Token_Lt, '=', Token_LtEq, '<', Token_ShiftLeft, '=', Token_SlAssign);
        CASE3B('>', Token_Gt, '=', Token_GtEq, '>', Token_ShiftRight, '=', Token_SrAssign);

        case '/': {
            result.kind = Token_Divide;
            result.value = string(1, tokenizer->scanner.data);
            advance_scanner(tokenizer);
            if (tokenizer->scanner.data[0] == '=') {
                result.kind = Token_DivAssign;
                ++result.value.size;
                advance_scanner(tokenizer);
            } else if (tokenizer->scanner.data[0] == '/') {
                do {
                    advance_scanner(tokenizer);
                } while (tokenizer->scanner.data[0] &&
                         (tokenizer->scanner.data[0] != '\n'));

                if (tokenizer->scanner.data[0] == '\n') {
                    advance_scanner(tokenizer);
                }
                result.indent = 0;
                goto repeat;
            }
        } break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            result.kind = Token_Integer;
            result.value = string(1, tokenizer->scanner.data);
            advance_scanner(tokenizer);

            b32 parseHex = false;
            if (result.value.data[0] == '0') {
                if ((to_lower_case(tokenizer->scanner.data[0]) == 'x') ||
                    (to_lower_case(tokenizer->scanner.data[0]) == 'b')) {
                    parseHex = (to_lower_case(tokenizer->scanner.data[0]) == 'x');
                    ++result.value.size;
                    advance_scanner(tokenizer);
                }
            }

            if (parseHex) {
                while (is_hex_digit(tokenizer->scanner.data[0])) {
                    ++result.value.size;
                    advance_scanner(tokenizer);
                }
            } else {
                while (is_digit(tokenizer->scanner.data[0])) {
                    ++result.value.size;
                    advance_scanner(tokenizer);
                }
            }

            if (tokenizer->scanner.data[0] == '.') {
                i_expect(parseHex == false);
                result.kind = Token_Float;
                ++result.value.size;
                advance_scanner(tokenizer);

                while (is_digit(tokenizer->scanner.data[0])) {
                    ++result.value.size;
                    advance_scanner(tokenizer);
                }
            }

            if ((tokenizer->scanner.data[0] == 'E') ||
                (tokenizer->scanner.data[0] == 'e')) {
                result.kind = Token_Float;
                ++result.value.size;
                advance_scanner(tokenizer);

                if ((tokenizer->scanner.data[0] == '+') ||
                    (tokenizer->scanner.data[0] == '-')) {
                    ++result.value.size;
                    advance_scanner(tokenizer);
                }

                while (is_digit(tokenizer->scanner.data[0])) {
                    ++result.value.size;
                    advance_scanner(tokenizer);
                }
            }
        } break;

        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
        case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
        case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
        case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
        case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z':
        case '_':
        {
            result.kind = Token_Name;
            result.value = string(1, tokenizer->scanner.data);
            advance_scanner(tokenizer);

            while (is_alnum(tokenizer->scanner.data[0]) ||
                   (tokenizer->scanner.data[0] == '_')) {
                ++result.value.size;
                advance_scanner(tokenizer);
            }
        } break;

        case '"': {
            result.kind = Token_String;
            advance_scanner(tokenizer);
            result.value = string(0, tokenizer->scanner.data);
            if (tokenizer->scanner.data[0] == '"') {
                tokenize_error(tokenizer, "Constant string cannot be empty.");
            }

            // TODO(michiel): Use proper end of line (\n\r or \r\n)
            while (tokenizer->scanner.data[0] &&
                   (tokenizer->scanner.data[0] != '"') &&
                   !is_end_of_line(tokenizer->scanner.data[0]))
            {
                if (tokenizer->scanner.data[0] == '\\')
                {
                    ++result.value.size;
                    advance_scanner(tokenizer);
                }
                // TODO(michiel): Handle invalid escape case
                ++result.value.size;
                advance_scanner(tokenizer);
            }
            if (tokenizer->scanner.data[0] != '"') {
                tokenize_error(tokenizer, "Unexpected ending of constant string.");
            }
            advance_scanner(tokenizer);
        } break;

        default: {
            if (is_printable(tokenizer->scanner.data[0])) {
                tokenize_error(tokenizer, "Unexpected character encountered '%c'.",
                               tokenizer->scanner.data[0]);
            } else {
                tokenize_error(tokenizer, "Unexpected character encountered '\\x%X'.",
                               tokenizer->scanner.data[0]);
            }
            advance_scanner(tokenizer);
        } break;
    }

    return result;
}

#undef CASE3B
#undef CASE2B
#undef CASE2
#undef CASE1

internal Token
peek_token(Tokenizer *tokenizer)
{
    Token result = {Token_None};
    Tokenizer temp = *tokenizer;
    result = get_token(tokenizer);
    *tokenizer = temp;
    return result;
}

internal b32
is_parsing(Tokenizer *tokenizer)
{
    b32 result;
    result = !tokenizer->error;
    return result;
}

internal b32
is_valid(Token token)
{
    b32 result = false;
    result = ((Token_None < token.kind) && (token.kind < TokenCount));
    return result;
}

internal b32
is_token_kind(Token token, TokenKind tokenKind)
{
    return token.kind == tokenKind;
}

internal b32
match_token(Tokenizer *tokenizer, TokenKind tokenKind)
{
    Token match = peek_token(tokenizer);
    b32 result = is_token_kind(match, tokenKind);
    if (result) {
        get_token(tokenizer);
    }
    return result;
}

internal b32
match_token_name(Tokenizer *tokenizer, String name)
{
    Token match = peek_token(tokenizer);
    b32 result = is_token_kind(match, Token_Name) && (match.value == name);
    if (result) {
        get_token(tokenizer);
    }
    return result;
}

internal Token
expect_token(Tokenizer *tokenizer, TokenKind tokenKind)
{
    Token result = get_token(tokenizer);
    if (!is_token_kind(result, tokenKind))
    {
        tokenize_error(tokenizer, "Expected '%.*s', got '%.*s'.",
                       STR_FMT(get_token_kind_name(tokenKind)),
                       STR_FMT(get_token_kind_name(result.kind)));
    }
    return result;
}

internal Token
expect_name(Tokenizer *tokenizer, String name)
{
    Token result = get_token(tokenizer);
    if (!is_token_kind(result, Token_Name) ||
        (result.value != name))
    {
        tokenize_error(tokenizer, "Expected name '%.*s', got '%.*s'.", STR_FMT(name),
                       STR_FMT(result.value));
    }
    return result;
}

internal Token
expect_integer_range(Tokenizer *tokenizer, s32 minValue, s32 maxValue)
{
    Token result = get_token(tokenizer);

    if (is_token_kind(result, Token_Integer))
    {
        result.s32 = safe_truncate_to_s32(number_from_string(result.value));
        if ((result.s32 >= minValue) &&
            (result.s32 <= maxValue))
        {
            // NOTE(michiel): Everything is fine
        }
        else
        {
            tokenize_error(tokenizer, "Expected integer in range(%d, %d), got '%d'.", minValue, maxValue, result.s32);
        }
    }
    else
    {
        tokenize_error(tokenizer, "Expected integer, got '%.*s'.", STR_FMT(get_token_kind_name(result.kind)));
    }

    return result;
}

internal void
print_token(Token token)
{
#if !COMPILER_MSVC
    if (token.kind == Token_Newline)
    {
        fprintf(stdout, "%.*s:%4d:%03d: %.*s: '\\n' (indent = %d)\n", STR_FMT(token.origin.filename),
                token.origin.line, token.origin.column, STR_FMT(get_token_kind_name(token.kind)),
                token.indent);
    }
    else
    {
        fprintf(stdout, "%.*s:%4d:%03d: %.*s: '%.*s' (indent = %d)\n", STR_FMT(token.origin.filename),
                token.origin.line, token.origin.column, STR_FMT(get_token_kind_name(token.kind)),
                STR_FMT(token.value), token.indent);
    }
#endif
}
