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
    i_expect(strings_are_equal_c(a, str_intern_c(interns, a)));
    i_expect(strings_are_equal(str_intern_c(interns, a), str_intern_c(interns, a)));
    i_expect(strings_are_equal(str_intern(interns, str_intern_c(interns, a)),
                               str_intern_c(interns, a)));
    
    char b[] = "Hallow";
    i_expect(a != b);
    i_expect(strings_are_equal(str_intern_c(interns, a), str_intern_c(interns, b)));
    char *c = "Hallow!";
    i_expect(!strings_are_equal(str_intern_c(interns, a), str_intern_c(interns, c)));
    char *d = "Hallow?";
    i_expect(!strings_are_equal(str_intern_c(interns, a), str_intern_c(interns, d)));
    i_expect(!strings_are_equal(str_intern_c(interns, c), str_intern_c(interns, d)));
    char *e = "Hall";
    i_expect(!strings_are_equal(str_intern_c(interns, a), str_intern_c(interns, e)));
    i_expect(!strings_are_equal(str_intern_c(interns, c), str_intern_c(interns, e)));
    i_expect(!strings_are_equal(str_intern_c(interns, d), str_intern_c(interns, e)));
    
    str_interns_free(interns);
}

int main(int argc, char **argv)
{
    buf_test();
    map_test(10000);
    str_intern_test();
}
