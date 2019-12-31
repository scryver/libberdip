//
// NOTE(michiel): Strings and string interning
//

global const char gDecimalChars[]  = "0123456789";
global const char gLowerHexChars[] = "0123456789abcdef";
global const char gUpperHexChars[] = "0123456789ABCDEF";

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

typedef Buffer String;

// TODO(michiel): Proper UTF8 support

// NOTE(michiel): String interning
typedef struct Interns
{
    Arena arena;
    Map map;
} Interns;

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
        result.data[result.size++] = to_lower_case(str.data[i]);
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
        result.data[i] = to_lower_case(str.data[i]);
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
        result.data[i] = to_upper_case(str.data[i]);
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

        result.data[index++] = toUpper ? to_upper_case(result.data[i]) : result.data[i];
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
        result.data[0] = to_upper_case(result.data[0]);
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

        result.data[result.size++] = (space || first) ? to_upper_case(str.data[i]) : str.data[i];
        first = false;
        space = !is_alpha(result.data[i]);
    }
    result.data[result.size] = 0;
    return result;
}

// NOTE(michiel): Use this to clean up string formatting arguments (for usage with "%.*s")
// TODO(michiel): Have our own printf with %S to support strings
#define STR_FMT(s)  safe_truncate_to_u32((s).size), (s).data

#define static_string(c) {sizeof(c) - 1, (u8 *)c}
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

#endif // __cplusplus

internal String
get_extension(String name)
{
    s32 dotPos = name.size - 1;
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
    s32 dotPos = name.size - 1;
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

internal void
str_interns_free(Interns *interns)
{
    arena_free(&interns->arena);
    map_free(&interns->map);
}

// NOTE(michiel): This struct is used internly to speed up the mapping process (->next)
//   and store the string data so every string.data pointer points to the same memory,
//   if they are the same string of course.
typedef struct InternedString
{
    struct InternedString *next;
    u32 size;
    char data[1]; // NOTE(michiel): Placeholder for the string data
} InternedString;

internal InternedString *
str_intern_(Interns *interns, String str)
{
    u64 hash = hash_bytes(str.data, str.size);
    u64 key = hash ? hash : 1;
    InternedString *intern = (InternedString *)map_u64_get(&interns->map, key);
    InternedString *it = intern;
    while (it) {
        String itStr = string(it->size, it->data);
#ifdef __cplusplus
        if (itStr == str)
#else
            if (strings_are_equal(itStr, str))
#endif
        {
            return it;
        }
        it = it->next;
    }

    // NOTE(michiel): No matching string found in the map, so add it
    umm newSize = offset_of(InternedString, data) + str.size + 1; // Add one for the 0-term
    InternedString *newInterned = (InternedString *)arena_allocate(&interns->arena, newSize);
    newInterned->next = intern;
    newInterned->size = str.size;
    copy(str.size, str.data, newInterned->data);
    newInterned->data[str.size] = 0;
    map_u64_put(&interns->map, key, newInterned);
    return newInterned;
}

internal InternedString *
str_intern_fmt_(Interns *interns, char *fmt, ...)
{
    static char buffer[4096];

    va_list args;
    va_start(args, fmt);
    u32 total = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    buffer[total] = 0;

    return str_intern_(interns, string(total, buffer));
}

internal String
str_intern(Interns *interns, String str)
{
    if (!str.size) {
        String result = {0, 0};
        return result;
    }
    InternedString *intStr = str_intern_(interns, str);
    return string(intStr->size, intStr->data);
}

internal String
vstr_intern_fmt(Interns *interns, char *fmt, va_list args)
{
    static char buffer[4096];
    u32 total = vsnprintf(buffer, sizeof(buffer), fmt, args);
    buffer[total] = 0;
    return str_intern(interns, string(total, buffer));
}

internal String
str_intern_fmt(Interns *interns, char *fmt, ...)
{
    String result = {};

    va_list args;
    va_start(args, fmt);
    result = vstr_intern_fmt(interns, fmt, args);
    va_end(args);

    return result;
}

#ifdef __cplusplus

internal InternedString *
str_intern_(Interns *interns, const char *str)
{
    return str_intern_(interns, string(str));
}

internal String
str_intern(Interns *interns, const char *str)
{
    return str_intern(interns, string(str));
}

#else

internal InternedString *
str_intern_c_(Interns *interns, const char *str)
{
    return str_intern_(interns, stringc(str));
}

internal String
str_intern_c(Interns *interns, const char *str)
{
    return str_intern(interns, stringc(str));
}

#endif

internal f64
float_from_string(String s)
{
    // TODO(michiel): Exponent support
    f64 result = 0.0;

    String scanner = s;
    while (scanner.size && is_digit(scanner.data[0]))
    {
        result *= 10.0;
        //result += (f64)(scanner.data[0] - '0');
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

    return result;
}

internal u32
parse_half_hex_byte(char c)
{
    i_expect(is_hex_digit(c));
    u32 result = c & 0xF;
    if (!is_digit(c))
    {
        result += 9;
    }
    return result;
}

internal s64
number_from_string(String s)
{
    s64 result = 0;
    s64 base = 10;
    if ((s.size > 2) &&
        (s.data[0] == '0'))
    {
        if ((s.data[1] == 'b') ||
            (s.data[1] == 'B'))
        {
            base = 2;
            --s.size;
            ++s.data;
        }
        else if ((s.data[1] == 'x') ||
                 (s.data[1] == 'X'))
        {
            base = 16;
            --s.size;
            ++s.data;
        }
        else
        {
            base = 8;
        }
        --s.size;
        ++s.data;
    }

    for (u32 sIdx = 0; sIdx < s.size; ++sIdx)
    {
        result *= base;
        s64 adding = parse_half_hex_byte(s.data[sIdx]);
        i_expect(adding >= 0);
        i_expect(adding < base);
        result += adding;
    }
    return result;
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

    return result;
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
