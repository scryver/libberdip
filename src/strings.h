//
// NOTE(michiel): Strings
//

global const char gDecimalChars[]  = "0123456789";
global const char gLowerHexChars[] = "0123456789abcdef";
global const char gUpperHexChars[] = "0123456789ABCDEF";
#if COMPILER_MSVC
global const u32 gNumFromHex[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
#else
global const u32 gNumFromHex[256] = {
    ['0'] = 0,
    ['1'] = 1,
    ['2'] = 2,
    ['3'] = 3,
    ['4'] = 4,
    ['5'] = 5,
    ['6'] = 6,
    ['7'] = 7,
    ['8'] = 8,
    ['9'] = 9,
    ['a'] = 10, ['A'] = 10,
    ['b'] = 11, ['B'] = 11,
    ['c'] = 12, ['C'] = 12,
    ['d'] = 13, ['D'] = 13,
    ['e'] = 14, ['E'] = 14,
    ['f'] = 15, ['F'] = 15,
};
#endif

internal void
copy(String source, String *dest)
{
    copy(source.size, source.data, dest->data);
    dest->size = source.size;
}

internal inline u32
string_length(const char *cString)
{
    u64 length = 0;
    if (cString) {
        while (cString && *cString++) {
            ++length;
        }
    }
    return safe_truncate_to_u32(length);
}

// TODO(michiel): Proper UTF8 support

internal b32
is_digit(u32 n)
{
    return (('0' <= n) && (n <= '9'));
}

internal b32
is_hex_digit(u32 n)
{
    return (is_digit(n) ||
            (('a' <= n) && (n <= 'f')) ||
            (('A' <= n) && (n <= 'F')));
}

internal b32
is_lower_case(u32 c)
{
    return (('a' <= c) && (c <= 'z'));
}

internal b32
is_upper_case(u32 c)
{
    return (('A' <= c) && (c <= 'Z'));
}

internal b32
is_alpha(u32 a)
{
    return is_lower_case(a) || is_upper_case(a);
}

internal b32
is_alnum(u32 a)
{
    return is_digit(a) || is_alpha(a);
}

internal b32
is_end_of_line(u32 s)
{
    return ((s == '\n') || (s == '\r'));
}

internal b32
is_whitespace(u32 s)
{
    return ((s == ' ') || (s == '\t') || (s == '\v') || (s == '\f') ||
            is_end_of_line(s));
}

internal b32
is_printable(u32 a)
{
    return (' ' <= a) && (a <= '~');
}

internal u32
to_lower_case(u32 x)
{
    if (is_upper_case(x)) {
        return x | 0x20;
    } else {
        return x;
    }
}

internal u32
to_upper_case(u32 x)
{
    if (is_lower_case(x)) {
        return x & ~0x20;
    } else {
        return x;
    }
}

internal b32
is_snake_case(String name)
{
    b32 result = true;
    for (u32 i = 0; i < name.size; ++i) {
        if (!is_lower_case(name.data[i]) &&
            (name.data[i] != '_'))
        {
            result = false;
            break;
        }
    }
    return result;
}

internal String
get_line(String input)
{
    String result = {0, input.data};

    while (input.size && !is_end_of_line(result.data[result.size])) {
        ++result.size;
        --input.size;
    }

    return result;
}

internal String
normalize(String str, u32 maxDestSize, u8 *dest)
{
    // NOTE(michiel): Remove all non alpha-numeric chars except single spaces,
    //   make sure it doesn't start with a space and lower all letters.
    i_expect(str.size < maxDestSize);

    String result = {0, dest};

    b32 first = true;
    b32 space = false;
    for (u32 i = 0; i < str.size; ++i) {
        if (!is_alnum(str.data[i])) {
            space = !first;
            continue;
        }

        if (space) {
            result.data[result.size++] = ' ';
            space = false;
        }
        result.data[result.size++] = (u8)to_lower_case(str.data[i]);
        first = false;
    }

    result.data[result.size] = 0;
    return result;
}

internal String
to_lower(String str, u32 maxDestSize, u8 *dest)
{
    i_expect(str.size < maxDestSize);

    String result = {str.size, dest};

    for (u32 i = 0; i < str.size; ++i) {
        result.data[i] = (u8)to_lower_case(str.data[i]);
    }

    result.data[result.size] = 0;
    return result;
}

internal String
to_upper(String str, u32 maxDestSize, u8 *dest)
{
    i_expect(str.size < maxDestSize);

    String result = {str.size, dest};

    for (u32 i = 0; i < str.size; ++i) {
        result.data[i] = (u8)to_upper_case(str.data[i]);
    }

    result.data[result.size] = 0;
    return result;
}

internal String
to_camel(String str, u32 maxDestSize, u8 *dest)
{
    // NOTE(michiel): Replace all non alpha-numeric chars with a capital letter for the
    //   next char
    String result = normalize(str, maxDestSize, dest);
    b32 toUpper = true;
    u32 index = 0;
    for (u32 i = 0; i < result.size; ++i) {
        if (result.data[i] == ' ') {
            toUpper = true;
            continue;
        }

        result.data[index++] = toUpper ? (u8)to_upper_case(result.data[i]) : result.data[i];
        toUpper = !is_alpha(result.data[i]);
    }

    result.data[index] = 0;
    result.size = index;
    return result;
}

internal String
to_snake(String str, u32 maxDestSize, u8 *dest)
{
    // NOTE(michiel): Replace all non alpha-numeric chars with an underscore
    String result = normalize(str, maxDestSize, dest);
    for (u32 i = 0; i < result.size; ++i) {
        if (result.data[i] == ' ') {
            result.data[i] = '_';
        }
    }
    return result;
}

internal String
titleize(String str, u32 maxDestSize, u8 *dest)
{
    // NOTE(michiel): Capitalize the first char
    i_expect(str.size < maxDestSize);

    String result = {0, dest};
    if (str.size) {
        copy(str.size, str.data, result.data);
        result.size = str.size;
        result.data[0] = (u8)to_upper_case(result.data[0]);
        result.data[result.size] = 0;
    }

    return result;
}

internal String
capitalize(String str, u32 maxDestSize, u8 *dest)
{
    // NOTE(michiel): Capitalize the next char after a space (and the first char)
    i_expect(str.size < maxDestSize);

    String result = {0, dest};

    b32 first = true;
    b32 space = false;
    for (u32 i = 0; i < str.size; ++i) {
        if (str.data[i] == ' ') {
            space = true;
            result.data[result.size++] = ' ';
            continue;
        }

        result.data[result.size++] = (space || first) ? (u8)to_upper_case(str.data[i]) : str.data[i];
        first = false;
        space = !is_alpha(result.data[i]);
    }
    result.data[result.size] = 0;
    return result;
}

// NOTE(michiel): Use this to clean up string formatting arguments (for usage with "%.*s")
// TODO(michiel): Have our own printf with %S to support strings,
// also support extras for upper/lower/snake/camel/capitalize/titleize etc
#define STR_FMT(s)  safe_truncate_to_u32((s).size), (s).data

#if COMPILER_MSVC
#define static_string(c) String{sizeof(c) - 1, (u8 *)c}
#else
#define static_string(c) (String){sizeof(c) - 1, (u8 *)c}
#endif

#define to_cstring(s)    ((char *)s.data)

internal String
string(umm size, const void *data)
{
    String result = {0, 0};
    result.size = size;
    result.data = (u8 *)data;
    return result;
}

#ifdef __cplusplus

internal String
string(u8 *cString)
{
    // NOTE(michiel): Grumble...
    return string(string_length((char *)cString), cString);
}

internal String
string(umm size, const char *data)
{
    // NOTE(michiel): F*ck const ;-)
    return string(size, (void *)data);
}

internal String
string(const char *cString)
{
    return string(string_length(cString), cString);
}

internal b32
operator ==(String a, String b)
{
    b32 result = (a.size == b.size);
    if (result) {
        if (a.data != b.data) {
            for (u32 index = 0; index < a.size; ++index) {
                if (a.data[index] != b.data[index]) {
                    result = false;
                    break;
                }
            }
        }
    }
    return result;
}

internal b32
operator !=(String a, String b)
{
    return !(a == b);
}

internal b32
strings_are_equal(umm size, const char *a, String b)
{
    return string(size, a) == b;
}

internal b32
strings_are_equal(const char *a, String b)
{
    return string(string_length(a), a) == b;
}

internal b32
strings_are_equal(String a, const char *b)
{
    return a == string(string_length(b), b);
}

internal b32
strings_are_equal(umm size, const char *a, const char *b)
{
    return string(size, a) == string(string_length(b), b);
}

internal b32
strings_are_equal(const char *a, const char *b)
{
    return string(string_length(a), a) == string(string_length(b), b);
}

internal b32
string_contains(String str, String subStr)
{
    b32 result = str.size && (subStr.size <= str.size);
    if (result) {
        if (str.data != subStr.data) {
            u32 curIndex = 0;
            result = false;
            while (curIndex <= (str.size - subStr.size)) {
                if (subStr.data[0] == str.data[curIndex]) {
                    String test = {subStr.size, str.data + curIndex};
                    if (test == subStr) {
                        result = true;
                        break;
                    }
                }
                ++curIndex;
            }
        }
    }
    return result;
}

internal b32
string_contains(const char *str, String subStr)
{
    return string_contains(string(string_length(str), str), subStr);
}

internal b32
string_contains(String str, const char *subStr)
{
    return string_contains(str, string(string_length(subStr), subStr));
}

internal b32
starts_with(String baseString, String expectedPrefix)
{
    b32 result = baseString.size >= expectedPrefix.size;
    if (result)
    {
        result = string(expectedPrefix.size, baseString.data) == expectedPrefix;
    }
    return result;
}

internal b32
ends_with(String baseString, String expectedSuffix)
{
    b32 result = baseString.size >= expectedSuffix.size;
    if (result)
    {
        result = string(expectedSuffix.size, baseString.data + baseString.size - expectedSuffix.size) == expectedSuffix;
    }
    return result;
}

#else

internal String
stringc(const char *cString)
{
    return string(string_length(cString), cString);
}

internal b32
strings_are_equal(String a, String b)
{
    b32 result = (a.size == b.size);
    if (result) {
        if (a.data != b.data) {
            for (u32 index = 0; index < a.size; ++index) {
                if (a.data[index] != b.data[index]) {
                    result = false;
                    break;
                }
            }
        }
    }
    return result;
}

internal b32
strings_are_equal_sc(umm size, const char *a, String b)
{
    return strings_are_equal(string(size, a), b);
}

internal b32
strings_are_equal_c(const char *a, String b)
{
    return strings_are_equal(string(string_length(a), a), b);
}

internal b32
string_contains(String str, String subStr)
{
    b32 result = (subStr.size <= str.size);
    if (result) {
        if (str.data != subStr.data) {
            u32 curIndex = 0;
            result = false;
            while (curIndex <= (str.size - subStr.size)) {
                if (subStr.data[0] == str.data[curIndex]) {
                    String test = {subStr.size, str.data + curIndex};
                    if (strings_are_equal(test, subStr)) {
                        result = true;
                        break;
                    }
                }
                ++curIndex;
            }
        }
    }
    return result;
}

internal b32
starts_with(String baseString, String expectedPrefix)
{
    b32 result = baseString.size >= expectedPrefix.size;
    if (result)
    {
        result = strings_are_equal(string(expectedPrefix.size, baseString.data), expectedPrefix);
    }
    return result;
}

internal b32
ends_with(String baseString, String expectedSuffix)
{
    b32 result = baseString.size >= expectedSuffix.size;
    if (result)
    {
        result = strings_are_equal(string(expectedSuffix.size, baseString.data + baseString.size - expectedSuffix.size), expectedSuffix);
    }
    return result;
}

#endif // __cplusplus

internal String
get_extension(String name)
{
    s32 dotPos = (s32)safe_truncate_to_u32(name.size) - 1;
    while (dotPos >= 0) {
        if (name.data[dotPos] == '.') {
            break;
        }
        --dotPos;
    }
    String result = {0, 0};
    if (dotPos >= 0) {
        result.size = name.size - dotPos - 1;
        result.data = name.data + dotPos + 1;
    }
    return result;
}

internal String
remove_extension(String name)
{
    s32 dotPos = (s32)safe_truncate_to_u32(name.size) - 1;
    while (dotPos >= 0) {
        if (name.data[dotPos] == '.') {
            break;
        }
        --dotPos;
    }
    String result = {0, 0};
    if (dotPos >= 0) {
        result.size = dotPos;
        result.data = name.data;
    }
    return result;
}

// NOTE(michiel): Yes, destSize sounds better than sizeDest... or aSize, bSize
internal String
string_concat(String a, String b, umm destSize, char *dest)
{
    String result = {0, (u8 *)dest};
    while ((result.size < a.size) && (result.size < destSize))
    {
        result.data[result.size] = a.data[result.size];
        ++result.size;
    }
    for (u32 j = 0; (j < b.size) && (result.size < destSize); ++j)
    {
        result.data[result.size++] = b.data[j];
    }
    if (result.size < destSize)
    {
        result.data[result.size] = 0;
    }
    return result;
}

#if !LIBBERDIP_NO_STDIO
internal String
vstring_fmt(u32 maxDestCount, u8 *dest, const char *fmt, va_list args)
{
    umm printed = vsnprintf((char *)dest, maxDestCount, fmt, args);
    String result = {safe_truncate_to_u32(printed), dest};
    i_expect(result.size < maxDestCount);
    result.data[result.size] = 0;
    return result;
}

internal String
string_fmt(u32 maxDestCount, u8 *dest, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    String result = vstring_fmt(maxDestCount, dest, fmt, args);
    va_end(args);
    return result;
}
#endif

internal String
append_string(String base, String suffix, u32 maxCount)
{
    for (u32 i = 0; i < suffix.size; ++i) {
        if (base.size < maxCount) {
            base.data[base.size++] = suffix.data[i];
        }
    }
    return base;
}

#if !LIBBERDIP_NO_STDIO
internal String
append_string_fmt(String base, u32 maxCount, const char *fmt, ...)
{
    i_expect(base.size < maxCount);
    va_list args;
    va_start(args, fmt);

    String result = base;
    String appended = vstring_fmt(maxCount - safe_truncate_to_u32(base.size), base.data + base.size, fmt, args);
    result.size += appended.size;

    return result;
}
#endif

internal f64
float_from_string(String s)
{
    f64 result = 0.0;

    String scanner = s;

    f64 sign = 1.0;
    if (scanner.size && (scanner.data[0] == '-'))
    {
        sign = -1.0;
        ++scanner.data;
        --scanner.size;
    }
    else if (scanner.size && (scanner.data[0] == '+'))
    {
        ++scanner.data;
        --scanner.size;
    }

    while (scanner.size && is_digit(scanner.data[0]))
    {
        result *= 10.0;
        result += (f64)(scanner.data[0] & 0xF);
        ++scanner.data;
        --scanner.size;
    }

    if (scanner.data[0] == '.') {
        f64 multiplier = 0.1;
        ++scanner.data;
        --scanner.size;
        while (scanner.size && is_digit(scanner.data[0])) {
            f64 addend = (f64)(scanner.data[0] & 0xF);
            addend *= multiplier;
            result += addend;
            multiplier *= 0.1;
            ++scanner.data;
            --scanner.size;
        }
    }

    if(scanner.size && to_lower_case(scanner.data[0]) == 'e')
    {
        ++scanner.data;
        --scanner.size;

        f64 exponentSign = 1.0;

        if(scanner.size && scanner.data[0] == '-')
        {
            exponentSign = -1.0;

            ++scanner.data;
            --scanner.size;
        }
        else if (scanner.size && scanner.data[0] == '+')
        {
            ++scanner.data;
            --scanner.size;
        }

        f64 exponent = 0;
        while (scanner.size && is_digit(scanner.data[0])) {
            f64 addend = (f64)(scanner.data[0] & 0xF);
            exponent = exponent * 10.0 + addend;
            ++scanner.data;
            --scanner.size;
        }

        result *= pow64(10.0, exponentSign * exponent);
    }

    return result * sign;
}

internal b32
try_float_from_string(String s, f64 *dest)
{
    b32 valid = false;
    f64 result = 0.0;

    String scanner = s;

    f64 sign = 1.0;
    if (scanner.size && (scanner.data[0] == '-'))
    {
        sign = -1.0;
        advance(&scanner);
    }
    else if (scanner.size && (scanner.data[0] == '+'))
    {
        advance(&scanner);
    }

    while (scanner.size && is_digit(scanner.data[0]))
    {
        valid = true;
        result *= 10.0;
        result += (f64)(scanner.data[0] & 0xF);
        advance(&scanner);
    }

    if (scanner.data[0] == '.') {
        f64 multiplier = 0.1;
        advance(&scanner);
        valid = false;
        while (scanner.size && is_digit(scanner.data[0])) {
            valid = true;
            f64 addend = (f64)(scanner.data[0] & 0xF);
            addend *= multiplier;
            result += addend;
            multiplier *= 0.1;
            advance(&scanner);
        }
    }

    if(scanner.size && to_lower_case(scanner.data[0]) == 'e')
    {
        advance(&scanner);
        valid = false;

        f64 exponentSign = 1.0;

        if(scanner.size && scanner.data[0] == '-')
        {
            exponentSign = -1.0;
            advance(&scanner);
        }
        else if (scanner.size && scanner.data[0] == '+')
        {
            advance(&scanner);
        }

        f64 exponent = 0;
        while (scanner.size && is_digit(scanner.data[0])) {
            valid = true;
            f64 addend = (f64)(scanner.data[0] & 0xF);
            exponent = exponent * 10.0 + addend;
            advance(&scanner);
        }

        result *= pow64(10.0, exponentSign * exponent);
    }

    valid = valid && (scanner.size == 0);
    if (valid)
    {
        *dest = sign * result;
    }

    return valid;
}

internal u32
parse_half_hex_byte(char c)
{
    i_expect(is_hex_digit(c));
    u32 result = gNumFromHex[(u8)c];
    return result;
}

internal b32
try_parse_half_hex_byte(char c, u32 *dest)
{
    b32 result = is_hex_digit(c);
    if(result) {
        *dest = gNumFromHex[(u8)c];
    }
    return result;
}

internal s32
s32_from_cstring_start(char *str)
{
    s32 result = 0;

    char *at = str;
    while (is_digit(*at))
    {
        result *= 10;
        result += (*at - '0');
        ++at;
    }
    return result;
}

internal s64
number_from_string(String s)
{
    b32 negative = s.size && (s.data[0] == '-');
    if (negative) {
        advance(&s);
    }
    s64 result = 0;
    s64 base = 10;
    if ((s.size > 2) &&
        (s.data[0] == '0'))
    {
        if ((s.data[1] == 'b') ||
            (s.data[1] == 'B'))
        {
            base = 2;
            advance(&s);
        }
        else if ((s.data[1] == 'x') ||
                 (s.data[1] == 'X'))
        {
            base = 16;
            advance(&s);
        }
        else
        {
            base = 8;
        }
        advance(&s);
    }

    for (u32 sIdx = 0; sIdx < s.size; ++sIdx)
    {
        result *= base;
        s64 adding = parse_half_hex_byte(s.data[sIdx]);
        i_expect(adding >= 0);
        i_expect(adding < base);
        result += adding;
    }
    return negative ? -result : result;
}

internal b32
try_decimal_from_string(String s, s64 *dest)
{
    b32 negative = s.size && (s.data[0] == '-');
    if (negative) {
        advance(&s);
    }
    b32 isValid = false;
    s64 result = 0;

    while (s.size && is_digit(s.data[0]))
    {
        isValid = true;
        result *= 10;
        result += s.data[0] - '0';
        advance(&s);
    }

    if (isValid)
    {
        *dest = negative ? -result : result;
    }
    return isValid;
}

internal u64
hex_from_string(String s)
{
    u64 result = 0;
    for (u32 sIdx = 0; sIdx < s.size; ++sIdx)
    {
        result *= 16;
        u64 adding = parse_half_hex_byte(s.data[sIdx]);
        i_expect(adding < 16);
        result += adding;
    }
    return result;
}

internal char
hex_from_u4(u8 data)
{
    i_expect((data & 0xF) == data);
    char result = gLowerHexChars[data];
    return result;
}

internal u8
u8_from_hex(char *hex)
{
    u32 result = 0;

    result = parse_half_hex_byte(*hex++) << 4;
    result |= parse_half_hex_byte(*hex);

    return safe_truncate_to_u8(result);
}

internal u16
u16_from_hex(char *hex)
{
    u16 result = 0;

    result = u8_from_hex(hex) << 8;
    result |= u8_from_hex(hex + 2);

    return result;
}

internal u32
u32_from_hex(char *hex)
{
    u32 result = 0;

    result = u16_from_hex(hex) << 16;
    result |= u16_from_hex(hex + 4);

    return result;
}

internal Buffer
hex_to_bytes(String hex, umm destLength, u8 *dest)
{
    i_expect((hex.size & ~1) == hex.size);
    i_expect((hex.size >> 1) < destLength);
    Buffer result = {0, dest};
    for (umm hIndex = 0; hIndex < hex.size; hIndex += 2)
    {
        result.data[result.size++] = u8_from_hex((char *)(hex.data + hIndex));
    }

    i_expect(result.size < destLength);
    return result;
}

internal String
bytes_to_hex(Buffer bytes, umm destLength, u8 *dest)
{
    i_expect((bytes.size << 1) < destLength);
    String result = {0, dest};
    for (umm bIndex = 0; bIndex < bytes.size; ++bIndex)
    {
        result.data[result.size++] = hex_from_u4(bytes.data[bIndex] >> 4);
        result.data[result.size++] = hex_from_u4(bytes.data[bIndex] & 0xF);
    }

    i_expect(result.size < destLength);
    result.data[result.size] = 0;
    return result;
}

enum PatternMatchFlag
{
    PatternMatchFlag_None      = 0x00,
    PatternMatchFlag_MaySkip   = 0x01,
    PatternMatchFlag_Restarted = 0x02,
};

// NOTE(michiel): Very simple regex matcher
// Only * as wildcard supported
internal b32
match_pattern(String pattern, String str)
{
    b32 result = false;
    u32 flags = PatternMatchFlag_None;
    String p = pattern;
    String s = str;
    while (s.size)
    {
        if (p.data[0] == '*')
        {
            flags = PatternMatchFlag_MaySkip;
            ++p.data;
            --p.size;
        }
        else if (flags == PatternMatchFlag_MaySkip)
        {
            result = true;
            if (s.data[0] == p.data[0])
            {
                flags = PatternMatchFlag_None;
                ++p.data;
                --p.size;
                ++s.data;
                --s.size;
            }
            else
            {
                flags = PatternMatchFlag_MaySkip;
                ++s.data;
                --s.size;
            }
        }
        else if (s.data[0] != p.data[0])
        {
            if (flags == PatternMatchFlag_Restarted)
            {
                break;
            }
            result = false;
            flags = PatternMatchFlag_Restarted;
            p = pattern;
        }
        else
        {
            result = true;
            flags = PatternMatchFlag_None;
            ++p.data;
            --p.size;
            ++s.data;
            --s.size;
        }
    }
    return result && (p.data[0] == 0);
}

internal b32
#ifdef __cplusplus
match_pattern(char *pattern, char *str)
#else
match_pattern_c(char *pattern, char *str)
#endif
{
    b32 result = false;
    result = match_pattern(string(string_length(pattern), pattern),
                           string(string_length(str), str));
    return result;
}

#if !LIBBERDIP_NO_STDIO
internal String
string_from_ip4(u32 ip4addr, u32 maxDataCount, u8 *data)
{
    String result = string_fmt(maxDataCount, data, "%u.%u.%u.%u",
                               (ip4addr >>  0) & 0xFF,
                               (ip4addr >>  8) & 0xFF,
                               (ip4addr >> 16) & 0xFF,
                               (ip4addr >> 24) & 0xFF);
    return result;
}

internal String
string_from_ip6(void *ip6src, u32 maxDataCount, u8 *data)
{
    u8 *ip6ptr = (u8 *)ip6src;

    u16 ipBlocks[8];
    ipBlocks[0] = ((u16)ip6ptr[ 0] << 8) | ip6ptr[ 1];
    ipBlocks[1] = ((u16)ip6ptr[ 2] << 8) | ip6ptr[ 3];
    ipBlocks[2] = ((u16)ip6ptr[ 4] << 8) | ip6ptr[ 5];
    ipBlocks[3] = ((u16)ip6ptr[ 6] << 8) | ip6ptr[ 7];
    ipBlocks[4] = ((u16)ip6ptr[ 8] << 8) | ip6ptr[ 9];
    ipBlocks[5] = ((u16)ip6ptr[10] << 8) | ip6ptr[11];
    ipBlocks[6] = ((u16)ip6ptr[12] << 8) | ip6ptr[13];
    ipBlocks[7] = ((u16)ip6ptr[14] << 8) | ip6ptr[15];

    u32 maxZeroRange = 0;
    u32 zeroRangeStart = 0;

    u32 zeroRangeCount = 0;
    u32 zeroRangeTest = 0;
    for (u32 idx = 0; idx < 8; ++idx)
    {
        if (ipBlocks[idx] == 0)
        {
            if (zeroRangeCount == 0)
            {
                zeroRangeTest = idx;
            }
            ++zeroRangeCount;
        }
        else if (zeroRangeCount > 1)
        {
            if (maxZeroRange < zeroRangeCount)
            {
                maxZeroRange = zeroRangeCount;
                zeroRangeStart = zeroRangeTest;
            }
            zeroRangeCount = 0;
        }
        else
        {
            zeroRangeCount = 0;
        }
    }

    if (zeroRangeCount > 1)
    {
        if (maxZeroRange < zeroRangeCount)
        {
            maxZeroRange = zeroRangeCount;
            zeroRangeStart = zeroRangeTest;
        }
    }

    String result = {0, data};
    if (maxZeroRange)
    {
        b32 doIp4 = false;

#if 0
        if (((zeroRangeStart == 0) && (maxZeroRange == 5) && (ipBlocks[5] == 0xFFFF)) ||
            ((zeroRangeStart == 0) && (maxZeroRange == 4) && (ipBlocks[4] == 0xFFFF) && (ipBlocks[5] == 0)))
        {
            doIp4 = true;
        }
#else
        if ((zeroRangeStart == 0) && (maxZeroRange == 5) && (ipBlocks[5] == 0xFFFF))
        {
            doIp4 = true;
        }
#endif

        u32 maxCount = doIp4 ? 6 : 8;

        for (u32 idx = 0; idx < maxCount; ++idx)
        {
            if (idx == zeroRangeStart)
            {
                result = append_string(result, string(2, "::"), maxDataCount);
            }
            else if ((idx > zeroRangeStart) &&
                     (idx < (zeroRangeStart + maxZeroRange)))
            {
                continue;
            }
            else
            {
                if ((idx > 0) && (result.data[result.size - 1] != ':'))
                {
                    result = append_string(result, string(1, ":"), maxDataCount);
                }
                result = append_string_fmt(result, maxDataCount, "%x", ipBlocks[idx]);
            }
        }

        if (doIp4)
        {
            u8 buf[16];
            String ip4 = string_from_ip4(*(u32 *)(ip6ptr + 12), array_count(buf), buf);
            result = append_string_fmt(result, maxDataCount, ":%.*s", STR_FMT(ip4));
        }

        if (result.size < maxDataCount)
        {
            result.data[result.size] = 0;
        }
    }
    else
    {
        result = string_fmt(maxDataCount, (u8 *)data, "%x:%x:%x:%x:%x:%x:%x:%x",
                            ipBlocks[0], ipBlocks[1], ipBlocks[2], ipBlocks[3],
                            ipBlocks[4], ipBlocks[5], ipBlocks[6], ipBlocks[7]);
    }

    return result;
}
#endif

internal u32
utf8_codepoint_size(String str)
{
    u32 result = 0;

    if (str.size)
    {
        if ((str.data[0] & 0xF8) == 0xF0)
        {
            result = 4;
        }
        else if ((str.data[0] & 0xF0) == 0xE0)
        {
            result = 3;
        }
        else if ((str.data[0] & 0xE0) == 0xC0)
        {
            result = 2;
        }
        else
        {
            result = 1;
        }
    }

    return result;
}

internal u32
codepoint_from_utf8(String str)
{
    u32 result = 0;

    if (str.size) {
        result = (u32)str.data[0];
        if ((result & 0xF8) == 0xF0)
        {
            if ((str.size > 3) &&
                (((str.data[1] & 0xC0) &
                  (str.data[2] & 0xC0) &
                  (str.data[3] & 0xC0)) == 0x80))
            {
                result = ((result & ~0xF8) << 6) | (str.data[1] & ~0xC0);
                result = (result << 6) | (str.data[2] & ~0xC0);
                result = (result << 6) | (str.data[3] & ~0xC0);
            }
            else
            {
                result = 0;
            }
        }
        else if ((result & 0xF0) == 0xE0)
        {
            if ((str.size > 2) &&
                (((str.data[1] & 0xC0) &
                  (str.data[2] & 0xC0)) == 0x80))
            {
                result = ((result & ~0xF0) << 6) | (str.data[1] & ~0xC0);
                result = (result << 6) | (str.data[2] & ~0xC0);
            }
            else
            {
                result = 0;
            }
        }
        else if ((result & 0xE0) == 0xC0)
        {
            if ((str.size > 1) &&
                ((str.data[1] & 0xC0) == 0x80))
            {
                result = ((result & ~0xE0) << 6) | (str.data[1] & ~0xC0);
            }
            else
            {
                result = 0;
            }
        }
        else if ((result & 0x80) != 0x00)
        {
            result = 0;
        }
    }

    return result;
}

internal u32
codepoint_from_utf8_advance(String *str)
{
    u32 result = 0;

    if (str->size) {
        result = (u32)str->data[0];
        if ((result & 0xF8) == 0xF0)
        {
            if ((str->size > 3) &&
                (((str->data[1] & 0xC0) &
                  (str->data[2] & 0xC0) &
                  (str->data[3] & 0xC0)) == 0x80))
            {
                result = ((result & ~0xF8) << 6) | (str->data[1] & ~0xC0);
                result = (result << 6) | (str->data[2] & ~0xC0);
                result = (result << 6) | (str->data[3] & ~0xC0);
                advance(str, 4);
            }
            else
            {
                result = 0;
                advance(str, 1);
            }
        }
        else if ((result & 0xF0) == 0xE0)
        {
            if ((str->size > 2) &&
                (((str->data[1] & 0xC0) &
                  (str->data[2] & 0xC0)) == 0x80))
            {
                result = ((result & ~0xF0) << 6) | (str->data[1] & ~0xC0);
                result = (result << 6) | (str->data[2] & ~0xC0);
                advance(str, 3);
            }
            else
            {
                result = 0;
                advance(str, 1);
            }
        }
        else if ((result & 0xE0) == 0xC0)
        {
            if ((str->size > 1) &&
                ((str->data[1] & 0xC0) == 0x80))
            {
                result = ((result & ~0xE0) << 6) | (str->data[1] & ~0xC0);
                advance(str, 2);
            }
            else
            {
                result = 0;
                advance(str, 1);
            }
        }
        else if ((result & 0x80) != 0x00)
        {
            advance(str, 1);
            result = 0;
        }
        else
        {
            advance(str, 1);
        }
    }

    return result;
}

internal String
trim_leading_whitespace(String s)
{
    // NOTE(michiel): Get a substring of the original input (data of result is owned by the original string!)
    String result = s;
    while (result.size && is_whitespace(result.data[0]))
    {
        advance(&result);
    }
    return result;
}

internal b32
trim_leading_whitespace(String *s)
{
    // NOTE(michiel): Modifies the string while keeping the data pointer the same (so moves memory if needed)
    u32 whitespaceCount = 0;
    while ((whitespaceCount < s->size) && is_whitespace(s->data[whitespaceCount]))
    {
        ++whitespaceCount;
    }
    if (whitespaceCount)
    {
        for (u32 idx = whitespaceCount; idx < s->size; ++idx)
        {
            s->data[idx - whitespaceCount] = s->data[idx];
        }
    }
    s->size = s->size - whitespaceCount;
    return whitespaceCount > 0;
}
