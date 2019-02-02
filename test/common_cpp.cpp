#include "../src/common.h"

internal void
buf_test(void)
{
    char *buf = 0;
    buf_printf(buf, "int: %d\n", 123);
    buf_printf(buf, "ptr: %p\n", buf);
    printf("%s", buf);
}

internal void
map_test(umm size) {
    Map map = {0};
    s32 n = size;
    for (u64 i = 1; i < n; ++i) {
        map_u64_put_u64(&map, i, i-1);
    }
    for (u64 i = 1; i < n; ++i) {
        u64 val = map_u64_get_u64(&map, i);
        i_expect(val == i - 1);
    }
    map_free(&map);
}

internal void
str_intern_test(void)
{
    Interns interns_ = {0};
    Interns *interns = &interns_;
    
    char *a = "Hallow";
    i_expect(strings_are_equal(a, str_intern(interns, a)));
    i_expect(str_intern(interns, a) == str_intern(interns, a));
    i_expect(str_intern(interns, str_intern(interns, a)) == str_intern(interns, a));
    
    char b[] = "Hallow";
    i_expect(a != b);
    i_expect(str_intern(interns, a) == str_intern(interns, b));
    char *c = "Hallow!";
    i_expect(str_intern(interns, a) != str_intern(interns, c));
    char *d = "Hallow?";
    i_expect(str_intern(interns, a) != str_intern(interns, d));
    i_expect(str_intern(interns, c) != str_intern(interns, d));
    char *e = "Hall";
    i_expect(str_intern(interns, a) != str_intern(interns, e));
    i_expect(str_intern(interns, c) != str_intern(interns, e));
    i_expect(str_intern(interns, d) != str_intern(interns, e));
    
    str_interns_free(interns);
}

int main(int argc, char **argv)
{
    fprintf(stdout, "Exten : %.*s\n", STR_FMT(get_extension(string("test.bla"))));
    fprintf(stdout, "Exten : %.*s\n", STR_FMT(get_extension(string("testbla"))));
    fprintf(stdout, "Exten : %.*s\n", STR_FMT(get_extension(string("tes.t.la"))));
    fprintf(stdout, "Exten : %.*s\n", STR_FMT(get_extension(string(".testbla"))));
    fprintf(stdout, "Exten : %.*s\n", STR_FMT(get_extension(string("testbla."))));
    char testStrange[256] = "../../test.abcdef";
    String strange = string(testStrange);
    fprintf(stdout, "Exten : %.*s (%lu)\n", STR_FMT(get_extension(strange)),
            get_extension(strange).size);
    
    String testStr = string("testing something 1230(%@$!*(!#^% and more");
    fprintf(stdout, "Base  : %.*s\n", STR_FMT(testStr));
    fprintf(stdout, "Normal: %.*s\n", STR_FMT(normalize(testStr)));
    fprintf(stdout, "Lower : %.*s\n", STR_FMT(to_lower(testStr)));
    fprintf(stdout, "Upper : %.*s\n", STR_FMT(to_upper(testStr)));
    fprintf(stdout, "Camel : %.*s\n", STR_FMT(to_camel(testStr)));
    fprintf(stdout, "Snake : %.*s\n", STR_FMT(to_snake(testStr)));
fprintf(stdout, "Title : %.*s\n", STR_FMT(titleize(testStr)));
fprintf(stdout, "Caps  : %.*s\n", STR_FMT(capitalize(testStr)));
    
    Interns tests = {0};
    String formatting = str_intern_fmt(&tests, "%.*s_%.*s", STR_FMT(string("one")),
                                       STR_FMT(string("two")));
    fprintf(stdout, "Test str: %.*s\n", STR_FMT(formatting));
    str_interns_free(&tests);
                
    char testBuf[256] = "Onish";
    String testBufStr = string(testBuf);
    testBufStr = append_string(testBufStr, string("_twish"), array_count(testBuf));
    fprintf(stdout, "Morish: %.*s\n", STR_FMT(testBufStr));
    
    buf_test();
    map_test(100);
    str_intern_test();
}
