internal void
test_vformat(const char *type, const char *fmt, va_list args)
{
    fprintf(stderr, "TEST::%s: ", type);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

internal void
test_format(const char *name, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    test_vformat(name, fmt, args);
    va_end(args);
}

internal void
test_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    test_vformat("ERROR  ", fmt, args);
    va_end(args);
}

internal void
test_fatal(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    test_vformat("FAILURE", fmt, args);
    va_end(args);
    exit(1);
}

#ifdef __cplusplus
#include "./tests_cpp.h"

//
// NOTE(michiel): C++-test interface
//

#define i_expect_equal(a, b) i_expect_equal_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less(a, b) i_expect_less_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater(a, b) i_expect_less_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal(a, b) i_expect_less_or_equal_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal(a, b) i_expect_greater_or_equal_(__FILE__, __LINE__, #a, #b, a, b)


//
// NOTE(michiel): End of C++-test interface
//

#else

#include "./tests_c.h"

//
// NOTE(michiel): C-test interface, difference here are the types, no overloading
//

#define i_expect_equal_u8(a, b) i_expect_equal_u8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_u16(a, b) i_expect_equal_u16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_u32(a, b) i_expect_equal_u32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_u64(a, b) i_expect_equal_u64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_s8(a, b) i_expect_equal_s8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_s16(a, b) i_expect_equal_s16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_s32(a, b) i_expect_equal_s32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_s64(a, b) i_expect_equal_s64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_b8(a, b) i_expect_equal_b8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_b16(a, b) i_expect_equal_b16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_b32(a, b) i_expect_equal_b32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_b64(a, b) i_expect_equal_b64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_f32(a, b) i_expect_equal_f32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_equal_f64(a, b) i_expect_equal_f64_(__FILE__, __LINE__, #a, #b, a, b)

#define i_expect_less_u8(a, b) i_expect_less_u8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_u16(a, b) i_expect_less_u16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_u32(a, b) i_expect_less_u32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_u64(a, b) i_expect_less_u64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_s8(a, b) i_expect_less_s8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_s16(a, b) i_expect_less_s16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_s32(a, b) i_expect_less_s32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_s64(a, b) i_expect_less_s64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_b8(a, b) i_expect_less_b8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_b16(a, b) i_expect_less_b16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_b32(a, b) i_expect_less_b32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_b64(a, b) i_expect_less_b64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_f32(a, b) i_expect_less_f32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_f64(a, b) i_expect_less_f64_(__FILE__, __LINE__, #a, #b, a, b)

#define i_expect_greater_u8(a, b) i_expect_greater_u8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_u16(a, b) i_expect_greater_u16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_u32(a, b) i_expect_greater_u32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_u64(a, b) i_expect_greater_u64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_s8(a, b) i_expect_greater_s8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_s16(a, b) i_expect_greater_s16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_s32(a, b) i_expect_greater_s32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_s64(a, b) i_expect_greater_s64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_b8(a, b) i_expect_greater_b8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_b16(a, b) i_expect_greater_b16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_b32(a, b) i_expect_greater_b32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_b64(a, b) i_expect_greater_b64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_f32(a, b) i_expect_greater_f32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_f64(a, b) i_expect_greater_f64_(__FILE__, __LINE__, #a, #b, a, b)

#define i_expect_less_or_equal_u8(a, b) i_expect_less_or_equal_u8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_u16(a, b) i_expect_less_or_equal_u16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_u32(a, b) i_expect_less_or_equal_u32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_u64(a, b) i_expect_less_or_equal_u64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_s8(a, b) i_expect_less_or_equal_s8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_s16(a, b) i_expect_less_or_equal_s16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_s32(a, b) i_expect_less_or_equal_s32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_s64(a, b) i_expect_less_or_equal_s64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_b8(a, b) i_expect_less_or_equal_b8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_b16(a, b) i_expect_less_or_equal_b16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_b32(a, b) i_expect_less_or_equal_b32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_b64(a, b) i_expect_less_or_equal_b64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_f32(a, b) i_expect_less_or_equal_f32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_less_or_equal_f64(a, b) i_expect_less_or_equal_f64_(__FILE__, __LINE__, #a, #b, a, b)

#define i_expect_greater_or_equal_u8(a, b) i_expect_greater_or_equal_u8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_u16(a, b) i_expect_greater_or_equal_u16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_u32(a, b) i_expect_greater_or_equal_u32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_u64(a, b) i_expect_greater_or_equal_u64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_s8(a, b) i_expect_greater_or_equal_s8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_s16(a, b) i_expect_greater_or_equal_s16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_s32(a, b) i_expect_greater_or_equal_s32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_s64(a, b) i_expect_greater_or_equal_s64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_b8(a, b) i_expect_greater_or_equal_b8_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_b16(a, b) i_expect_greater_or_equal_b16_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_b32(a, b) i_expect_greater_or_equal_b32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_b64(a, b) i_expect_greater_or_equal_b64_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_f32(a, b) i_expect_greater_or_equal_f32_(__FILE__, __LINE__, #a, #b, a, b)
#define i_expect_greater_or_equal_f64(a, b) i_expect_greater_or_equal_f64_(__FILE__, __LINE__, #a, #b, a, b)

//
// NOTE(michiel): End of C-test interface
//

#endif 

GEN_TEST_EQUAL(u, 8, u)
GEN_TEST_EQUAL(u, 16, u)
GEN_TEST_EQUAL(u, 32, u)
GEN_TEST_EQUAL(u, 64, llu)
GEN_TEST_EQUAL(s, 8, d)
GEN_TEST_EQUAL(s, 16, d)
GEN_TEST_EQUAL(s, 32, d)
GEN_TEST_EQUAL(s, 64, lld)
GEN_TEST_EQUAL(f, 32, f)
GEN_TEST_EQUAL(f, 64, f)
#undef GEN_TEST_EQUAL

GEN_TEST_BOOL_EQUAL(8)
GEN_TEST_BOOL_EQUAL(16)
GEN_TEST_BOOL_EQUAL(32)
GEN_TEST_BOOL_EQUAL(64)
#undef GEN_TEST_BOOL_EQUAL

GEN_TEST_LESS(u, 8, u)
GEN_TEST_LESS(u, 16, u)
GEN_TEST_LESS(u, 32, u)
GEN_TEST_LESS(u, 64, llu)
GEN_TEST_LESS(s, 8, d)
GEN_TEST_LESS(s, 16, d)
GEN_TEST_LESS(s, 32, d)
GEN_TEST_LESS(s, 64, lld)
GEN_TEST_LESS(f, 32, f)
GEN_TEST_LESS(f, 64, f)
#undef GEN_TEST_LESS

GEN_TEST_GREATER(u, 8, u)
GEN_TEST_GREATER(u, 16, u)
GEN_TEST_GREATER(u, 32, u)
GEN_TEST_GREATER(u, 64, llu)
GEN_TEST_GREATER(s, 8, d)
GEN_TEST_GREATER(s, 16, d)
GEN_TEST_GREATER(s, 32, d)
GEN_TEST_GREATER(s, 64, lld)
GEN_TEST_GREATER(f, 32, f)
GEN_TEST_GREATER(f, 64, f)
#undef GEN_TEST_GREATER

GEN_TEST_LESS_EQUAL(u, 8, u)
GEN_TEST_LESS_EQUAL(u, 16, u)
GEN_TEST_LESS_EQUAL(u, 32, u)
GEN_TEST_LESS_EQUAL(u, 64, llu)
GEN_TEST_LESS_EQUAL(s, 8, d)
GEN_TEST_LESS_EQUAL(s, 16, d)
GEN_TEST_LESS_EQUAL(s, 32, d)
GEN_TEST_LESS_EQUAL(s, 64, lld)
GEN_TEST_LESS_EQUAL(f, 32, f)
GEN_TEST_LESS_EQUAL(f, 64, f)
#undef GEN_TEST_LESS_EQUAL

GEN_TEST_GREATER_EQUAL(u, 8, u)
GEN_TEST_GREATER_EQUAL(u, 16, u)
GEN_TEST_GREATER_EQUAL(u, 32, u)
GEN_TEST_GREATER_EQUAL(u, 64, llu)
GEN_TEST_GREATER_EQUAL(s, 8, d)
GEN_TEST_GREATER_EQUAL(s, 16, d)
GEN_TEST_GREATER_EQUAL(s, 32, d)
GEN_TEST_GREATER_EQUAL(s, 64, lld)
GEN_TEST_GREATER_EQUAL(f, 32, f)
GEN_TEST_GREATER_EQUAL(f, 64, f)
#undef GEN_TEST_GREATER_EQUAL

