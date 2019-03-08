//
// NOTE(michiel): Strings and string interning
//
typedef Buffer String;

// TODO(michiel): Proper UTF8 support

// NOTE(michiel): String interning
typedef struct Interns
{
    Arena arena;
    Map map;
} Interns;

internal inline b32
is_digit(char n)
{
    return (('0' <= n) && (n <= '9'));
}

internal inline b32
is_hex_digit(char n)
{
    return (is_digit(n) ||
            (('a' <= n) && (n <= 'f')) ||
            (('A' <= n) && (n <= 'F')));
}

internal inline b32
is_lower_case(char c)
{
    return (('a' <= c) && (c <= 'z'));
}

internal inline b32
is_upper_case(char c)
{
    return (('A' <= c) && (c <= 'Z'));
}

internal inline b32
is_alpha(char a)
{
    return is_lower_case(a) || is_upper_case(a);
}

internal inline b32
is_alnum(char a)
{
    return is_digit(a) || is_alpha(a);
}

internal inline b32
is_space(char s)
{
    return ((s == ' ') || (s == '\n') || (s == '\r') ||
            (s == '\t') || (s == '\v'));
}

internal inline b32
is_printable(char a)
{
    return (' ' <= a) && (a <= '~');
}

internal inline char
to_lower_case(char x)
{
    if (is_upper_case(x)) {
        return x | 0x20;
    } else {
        return x;
    }
}

internal inline char
to_upper_case(char x)
{
    if (is_lower_case(x)) {
        return x & ~0x20;
    } else {
        return x;
    }
}

internal inline b32
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

// NOTE(michiel): This returns a pointer to a internal buffer!!!
internal String
normalize(String str)
{
    // NOTE(michiel): Remove all non alpha-numeric chars except single spaces,
    //   make sure it doesn't start with a space and lower all letters.
    persist u8 normalBuf[1024];
    i_expect(str.size < array_count(normalBuf));
    
    String result = {0};
    result.data = normalBuf;
    
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

// NOTE(michiel): This returns a pointer to a internal buffer!!!
internal String
to_lower(String str)
{
    persist u8 lowerBuf[1024];
    i_expect(str.size < array_count(lowerBuf));
    
    String result = {0};
    result.size = str.size;
    result.data = lowerBuf;
    
    for (u32 i = 0; i < str.size; ++i) {
        result.data[i] = to_lower_case(str.data[i]);
    }
    
    result.data[result.size] = 0;
    return result;
}

// NOTE(michiel): This returns a pointer to a internal buffer!!!
internal String
to_upper(String str)
{
    persist u8 upperBuf[1024];
    i_expect(str.size < array_count(upperBuf));
    
    String result = {0};
    result.size = str.size;
    result.data = upperBuf;
    
    for (u32 i = 0; i < str.size; ++i) {
        result.data[i] = to_upper_case(str.data[i]);
    }
    
    result.data[result.size] = 0;
    return result;
}

// NOTE(michiel): This returns a pointer to a internal buffer!!!
internal String
to_camel(String str)
{
    // NOTE(michiel): Replace all non alpha-numeric chars with a capital letter for the 
    //   next char
    String result = normalize(str);
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

// NOTE(michiel): This returns a pointer to a internal buffer!!!
internal String
to_snake(String str)
{
    // NOTE(michiel): Replace all non alpha-numeric chars with an underscore
    String result = normalize(str);
    for (u32 i = 0; i < result.size; ++i) {
        if (result.data[i] == ' ') {
            result.data[i] = '_';
        }
    }
    return result;
}

// NOTE(michiel): This returns a pointer to a internal buffer!!!
internal String
titleize(String str)
{
    // NOTE(michiel): Capitalize the first char
    persist u8 titleBuf[1024];
    i_expect(str.size < (array_count(titleBuf) - 1));
    
    String result = {0};
    if (!str.size) {
        return result;
    }
    result.data = titleBuf;
    
    copy(str.size, str.data, result.data);
    result.size = str.size;
    result.data[0] = to_upper_case(result.data[0]);
    result.data[result.size] = 0;
    return result;
}

// NOTE(michiel): This returns a pointer to a internal buffer!!!
internal String
capitalize(String str)
{
    // NOTE(michiel): Capitalize the next char after a space (and the first char)
    persist u8 titleBuf[1024];
    i_expect(str.size < (array_count(titleBuf) - 1));
    
    String result = {0};
    result.data = titleBuf;
    
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
        space = !is_alpha(result.data[i]);;
        }
    result.data[result.size] = 0;
    return result;
}

internal inline u32
string_length(const char *cString)
{
    u64 length = 0;
    while (*cString++) {
        ++length;
    }
    return safe_truncate_to_u32(length);
}

// NOTE(michiel): Use this to clean up string formatting arguments (for usage with "%.*s")
// TODO(michiel): Have our own printf with %S to support strings
#define STR_FMT(s)  safe_truncate_to_u32(s.size), s.data

#define static_string(c) {sizeof(c) - 1, (u8 *)c}
#define to_cstring(s)    ((char *)s.data)

internal inline String
string(umm size, const void *data)
{
    String result = {0};
    result.size = size;
    result.data = (u8 *)data;
    return result;
}

#ifdef __cplusplus

internal inline String
string(umm size, const char *data)
{
    // NOTE(michiel): F*ck const ;-)
    return string(size, (void *)data);
}

internal inline String
string(const char *cString)
{
    return string(string_length(cString), cString);
}

internal inline b32
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

internal inline b32
operator !=(String a, String b)
{
    return !(a == b);
}

internal inline b32
strings_are_equal(umm size, const char *a, String b)
{
    return string(size, a) == b;
}

internal inline b32
strings_are_equal(const char *a, String b)
{
    return string(string_length(a), a) == b;
}

internal inline b32
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

#else

internal inline String
stringc(const char *cString)
{
    return string(string_length(cString), cString);
}

internal inline b32
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

internal inline b32
strings_are_equal_sc(umm size, const char *a, String b)
{
    return strings_are_equal(string(size, a), b);
}

internal inline b32
strings_are_equal_c(const char *a, String b)
{
    return strings_are_equal(string(string_length(a), a), b);
}

internal inline b32
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
    String result = {0};
    if (dotPos >= 0) {
        result.size = name.size - dotPos - 1;
        result.data = name.data + dotPos + 1;
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
        String result = {0};
        return result;
    }
    InternedString *intStr = str_intern_(interns, str);
    return string(intStr->size, intStr->data);
}

internal String
str_intern_fmt(Interns *interns, char *fmt, ...)
{
    static char buffer[4096];
    
    va_list args;
    va_start(args, fmt);
    u32 total = vsnprintf(buffer, sizeof(buffer), fmt, args);
    buffer[total] = 0;
    va_end(args);
    
    return str_intern(interns, string(total, buffer));
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

internal s64
string_to_number(String s)
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
        s64 adding = 0;
        if (('0' <= s.data[sIdx]) &&
            (s.data[sIdx] <= '9'))
        {
            adding = s.data[sIdx] - '0';
        }
        else if (('a' <= s.data[sIdx]) && (s.data[sIdx] <= 'f'))
        {
            i_expect(base == 16);
            adding = (s.data[sIdx] - 'a') + 10;
        }
        else 
        {
            i_expect(('A' <= s.data[sIdx]) && (s.data[sIdx] <= 'F'));
            i_expect(base == 16);
            adding = (s.data[sIdx] - 'A') + 10;
        }
        i_expect(adding >= 0);
        i_expect(adding < base);
        result += adding;
    }
    return result;
}
