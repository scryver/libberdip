#include "../src/common.h"
#include "../src/maps.h"
#include "../src/strings.h"
#include "../src/tests.h"

TEST_BEGIN(buffer)
{
    char *buf = 0;
    buf_printf(buf, "int: %d\n", 123);
    buf_printf(buf, "ptr: %p\n", buf);
    //printf("%s", buf);
    i_expect_string_contains(stringc(buf), stringc("int: 123\n"));
    i_expect_string_contains(stringc(buf), stringc("ptr: 0x"));
}
TEST_END(buffer)

TEST_BEGIN(map)
{
    umm size = 10000;
    Map map = {0};
    for (u64 i = 1; i < size; ++i) {
        map_u64_put_u64(&map, i, i-1);
    }
    for (u64 i = 1; i < size; ++i) {
        u64 val = map_u64_get_u64(&map, i);
        i_expect_less_u64(val, size);
        i_expect_equal_u64(val, i - 1);
    }
    map_free(&map);
}
TEST_END(map)

TEST_BEGIN(string_intern)
{
    Interns interns_ = {0};
    Interns *interns = &interns_;
    
    char *a = "Hallow";
    i_expect(strings_are_equal_c(a, str_intern_c(interns, a)));
    i_expect_string_equal(str_intern_c(interns, a), str_intern_c(interns, a));
    i_expect_string_equal(str_intern(interns, str_intern_c(interns, a)),
                          str_intern_c(interns, a));
    
    char b[] = "Hallow";
    i_expect_ptr_not_equal(b, a);
    i_expect_string_equal(str_intern_c(interns, b), str_intern_c(interns, a));
    char *c = "Hallow!";
    i_expect_string_not_equal(str_intern_c(interns, c), str_intern_c(interns, a));
    char *d = "Hallow?";
    i_expect_string_not_equal(str_intern_c(interns, d), str_intern_c(interns, a));
    i_expect_string_not_equal(str_intern_c(interns, d), str_intern_c(interns, c));
    char *e = "Hall";
    i_expect_string_not_equal(str_intern_c(interns, e), str_intern_c(interns, a));
    i_expect_string_not_equal(str_intern_c(interns, e), str_intern_c(interns, c));
    i_expect_string_not_equal(str_intern_c(interns, e), str_intern_c(interns, d));
    
    str_interns_free(interns);
}
TEST_END(string_intern)

TEST_BEGIN(string_funcs)
{
    Interns testing = {0};
    String testStr = stringc("testing sOmething 1230(%@$!*(!#^%a and more");
    String normalStr = stringc("testing something 1230 a and more");
    String lowerStr = stringc("testing something 1230(%@$!*(!#^%a and more");
    String upperStr = stringc("TESTING SOMETHING 1230(%@$!*(!#^%A AND MORE");
    String camelStr = stringc("TestingSomething1230AAndMore");
    String snakeStr = stringc("testing_something_1230_a_and_more");
    String titleStr = stringc("Testing sOmething 1230(%@$!*(!#^%a and more");
    String capitStr = stringc("Testing SOmething 1230(%@$!*(!#^%a And More");
    
    i_expect_string_equal(normalize(testStr), normalStr);
    i_expect_string_equal(to_lower(testStr), lowerStr);
    i_expect_string_equal(to_upper(testStr), upperStr);
    i_expect_string_equal(to_camel(testStr), camelStr);
    i_expect_string_equal(to_snake(testStr), snakeStr);
    i_expect_string_equal(titleize(testStr), titleStr);
    i_expect_string_equal(capitalize(testStr), capitStr);
    
    /*
    fprintf(stdout, "Base  : %.*s\n", STR_FMT(testStr));
    fprintf(stdout, "Normal: %.*s\n", STR_FMT(normalize(testStr)));
    fprintf(stdout, "Lower : %.*s\n", STR_FMT(to_lower(testStr)));
    fprintf(stdout, "Upper : %.*s\n", STR_FMT(to_upper(testStr)));
    fprintf(stdout, "Camel : %.*s\n", STR_FMT(to_camel(testStr)));
    fprintf(stdout, "Snake : %.*s\n", STR_FMT(to_snake(testStr)));
    fprintf(stdout, "Title : %.*s\n", STR_FMT(titleize(testStr)));
    fprintf(stdout, "Caps  : %.*s\n", STR_FMT(capitalize(testStr)));
    */
    
    Interns tests = {0};
    String formatting = str_intern_fmt(&tests, "%.*s_%.*s", STR_FMT(stringc("one")),
                                       STR_FMT(stringc("two")));
    i_expect_string_equal(str_intern_c(&tests, "one_two"), formatting);
    //fprintf(stdout, "Test str: %.*s\n", STR_FMT(formatting));
    str_interns_free(&tests);
    
    char testBuf[256] = "Onish";
    String testBufStr = stringc(testBuf);
    testBufStr = append_string(testBufStr, stringc("_twish"), array_count(testBuf));
    i_expect_string_equal(testBufStr, (String)static_string("Onish_twish"));
    //fprintf(stdout, "Morish: %.*s\n", STR_FMT(testBufStr));
    
    str_interns_free(&testing);
}
TEST_END(string_funcs)

TEST_BEGIN(get_extension)
{
    Interns testing = {0};
    String firstTest = static_string("test.bla");
    String firstExt = str_intern_c(&testing, "bla");
    i_expect_string_equal(get_extension(firstTest), firstExt);
    
    String secTest = static_string("testbla");
    String secExt = {0, 0};
    String secExt2 = static_string("");
    i_expect_string_equal(get_extension(secTest), secExt);
    i_expect_string_equal(get_extension(secTest), secExt2);
    i_expect_string_not_equal(get_extension(secTest), firstExt);
    
    String thirdTest = static_string("tes.t.la");
    String thirdExt = static_string("la");
    i_expect_string_equal(get_extension(thirdTest), thirdExt);
    i_expect_string_not_equal(get_extension(thirdTest), firstExt);
    i_expect_string_not_equal(get_extension(thirdTest), secExt);
    
    String fourthTest = static_string(".testbla");
    String fourthExt = static_string("testbla");
    i_expect_string_equal(get_extension(fourthTest), fourthExt);
    i_expect_string_not_equal(get_extension(fourthTest), firstExt);
    i_expect_string_not_equal(get_extension(fourthTest), secExt);
    i_expect_string_not_equal(get_extension(fourthTest), thirdExt);
    
    String fiveTest = static_string("testbla.");
    i_expect_string_equal(get_extension(fiveTest), secExt);
    i_expect_string_not_equal(get_extension(fiveTest), firstExt);
    i_expect_string_not_equal(get_extension(fiveTest), thirdExt);
    i_expect_string_not_equal(get_extension(fiveTest), fourthExt);
    
    char testStrange[256] = "../../test.abcdef";
    String strange = stringc(testStrange);
    i_expect_string_equal(get_extension(strange), stringc("abcdef"));
    
    /*
    fprintf(stdout, "Exten : %.*s\n", STR_FMT(get_extension(string("test.bla"))));
    fprintf(stdout, "Exten : %.*s\n", STR_FMT(get_extension(string("testbla"))));
    fprintf(stdout, "Exten : %.*s\n", STR_FMT(get_extension(string("tes.t.la"))));
    fprintf(stdout, "Exten : %.*s\n", STR_FMT(get_extension(string(".testbla"))));
    fprintf(stdout, "Exten : %.*s\n", STR_FMT(get_extension(string("testbla."))));
    char testStrange[256] = "../../test.abcdef";
    String strange = string(testStrange);
    fprintf(stdout, "Exten : %.*s (%lu)\n", STR_FMT(get_extension(strange)),
            get_extension(strange).size);
            */
    
    str_interns_free(&testing);
}
TEST_END(get_extension)

#if 0
int main(int argc, char **argv)
{
    fprintf(stdout, "C Tests\n\n");
    testrun_buffer();
    testrun_map();
    testrun_string_intern();
    testrun_string_funcs();
    testrun_get_extension();
    fprintf(stdout, "\n\n");
}
#endif
