//
// NOTE(michiel): Implementation macros
//

#define GEN_TEST_EQUAL(type, bits, fmt) \
internal inline void \
i_expect_equal_(const char *file, u32 line, \
const char *nameA, const char *nameB, \
type##bits result, type##bits expectation) \
{ \
    if (!(result == expectation)) { \
        test_error("%s:%d: %s == %s", file, line, nameA, nameB); \
        test_fatal("Expected %" #fmt " got %" #fmt, expectation, result); \
    } \
}

#define GEN_TEST_NOT_EQUAL(type, bits, fmt) \
internal inline void \
i_expect_not_equal_(const char *file, u32 line, \
const char *nameA, const char *nameB, \
type##bits result, type##bits expectation) \
{ \
    if (!(result != expectation)) { \
        test_error("%s:%d: %s != %s", file, line, nameA, nameB); \
        test_fatal("Expected %" #fmt " got %" #fmt, expectation, result); \
    } \
}

#define GEN_TEST_ALMOST_EQUAL(type, bits, fmt) \
internal inline void \
i_expect_almost_equal_(const char *file, u32 line, \
const char *nameA, const char *nameB, \
type##bits result, type##bits expectation, \
type##bits epsilon) \
{ \
    if (!(((expectation - epsilon) < result) && (result < (expectation + epsilon)))) { \
        test_error("%s:%d: %s ~~ %s", file, line, nameA, nameB); \
        test_fatal("Expected %" #fmt "(+/- %" #fmt ") got %" #fmt, expectation, epsilon, \
        result); \
    } \
}

#define GEN_TEST_BOOL_EQUAL(bits) \
internal inline void \
i_expect_bool_equal_(const char *file, u32 line, \
const char *nameA, const char *nameB, \
b##bits result, b##bits expectation) \
{ \
    result = result ? 1 : 0; \
    expectation = expectation ? 1 : 0; \
    if (!(result == expectation)) { \
        test_error("%s:%d: %s == %s", file, line, nameA, nameB); \
        test_fatal("Expected %s got %s", expectation ? "true" : "false", \
        result ? "true" : "false"); \
    } \
}

#define GEN_TEST_BOOL_NOT_EQUAL(bits) \
internal inline void \
i_expect_not_bool_equal_(const char *file, u32 line, \
const char *nameA, const char *nameB, \
b##bits result, b##bits expectation) \
{ \
    result = result ? 1 : 0; \
    expectation = expectation ? 1 : 0; \
    if (!(result != expectation)) { \
        test_error("%s:%d: %s != %s", file, line, nameA, nameB); \
        test_fatal("Expected %s got %s", expectation ? "true" : "false", \
        result ? "true" : "false"); \
    } \
}

#define GEN_TEST_LESS(type, bits, fmt) \
internal inline void \
i_expect_less_(const char *file, u32 line, const char *nameA, const char *nameB, \
type##bits result, type##bits expectation) \
{ \
    if (!(result < expectation)) { \
        test_error("%s:%d: %s < %s", file, line, nameA, nameB); \
        test_fatal("Expected %" #fmt " to be less than %" #fmt, result, expectation); \
    } \
}

#define GEN_TEST_GREATER(type, bits, fmt) \
internal inline void \
i_expect_greater_(const char *file, u32 line, const char *nameA, const char *nameB, \
type##bits result, type##bits expectation) \
{ \
    if (!(result > expectation)) { \
        test_error("%s:%d: %s > %s", file, line, nameA, nameB); \
        test_fatal("Expected %" #fmt " to be greater than %" #fmt, result, expectation); \
    } \
}

#define GEN_TEST_LESS_EQUAL(type, bits, fmt) \
internal inline void \
i_expect_less_or_equal_(const char *file, u32 line, const char *nameA, const char *nameB, \
type##bits result, type##bits expectation) \
{ \
    if (!(result <= expectation)) { \
        test_error("%s:%d: %s <= %s", file, line, nameA, nameB); \
        test_fatal("Expected %" #fmt " to be less than %" #fmt, result, expectation); \
    } \
}

#define GEN_TEST_GREATER_EQUAL(type, bits, fmt) \
internal inline void \
i_expect_greater_or_equal_(const char *file, u32 line, const char *nameA, const char *nameB, \
type##bits result, type##bits expectation) \
{ \
    if (!(result >= expectation)) { \
        test_error("%s:%d: %s >= %s", file, line, nameA, nameB); \
        test_fatal("Expected %" #fmt " to be greater than %" #fmt, result, expectation); \
    } \
}

internal inline void
i_expect_equal_(const char *file, u32 line, const char *nameA, const char *nameB,
                        String result, String expectation)
{
    if (!(result == expectation)) {
        test_error("%s:%d: %s == %s", file, line, nameA, nameB);
        test_fatal("Expected '%.*s' got '%.*s'", STR_FMT(expectation), STR_FMT(result));
    }
}

internal inline void
i_expect_not_equal_(const char *file, u32 line, const char *nameA, const char *nameB,
                            String result, String expectation)
{
    if (result == expectation) {
        test_error("%s:%d: %s != %s", file, line, nameA, nameB);
        test_fatal("Expected '%.*s' got '%.*s'", STR_FMT(expectation), STR_FMT(result));
    }
}

internal inline void
i_expect_equal_(const char *file, u32 line, const char *nameA, const char *nameB,
                void *result, void *expectation)
{
    if (!(result == expectation)) {
        test_error("%s:%d: %s == %s", file, line, nameA, nameB);
        test_fatal("Expected %p got %p", expectation, result);
    }
}

internal inline void
i_expect_not_equal_(const char *file, u32 line, const char *nameA, const char *nameB,
                    void *result, void *expectation)
{
    if (!(result != expectation)) {
        test_error("%s:%d: %s == %s", file, line, nameA, nameB);
        test_fatal("Expected %p got %p", expectation, result);
    }
}

internal inline void
i_expect_contains_(const char *file, u32 line, const char *nameA, const char *nameB,
                  String result, String substring)
{
    if (!string_contains(result, substring)) {
        test_error("%s:%d: %s contains %s", file, line, nameA, nameB);
        test_fatal("Expected that '%.*s' contained '%.*s'", STR_FMT(result),
                   STR_FMT(substring));
    }
}

