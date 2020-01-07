
TEST_BEGIN(divmod)
{
    u32 a = 100;
    u32 b =  28;
    DivModU32 resultU32 = divmod(a, b);
    i_expect_equal(resultU32.div, a / b);
    i_expect_equal(resultU32.mod, a % b);

    s32 c = -100;
    s32 d =  28;
    DivModS32 resultS32 = divmod(c, d);
    i_expect_equal(resultS32.div, c / d);
    i_expect_equal(resultS32.mod, c % d);

    u64 e = 100235326;
    u64 f =  28;
    DivModU64 resultU64 = divmod(e, f);
    i_expect_equal(resultU64.div, e / f);
    i_expect_equal(resultU64.mod, e % f);

    s64 g = -100235326;
    s64 h =  28;
    DivModS64 resultS64 = divmod(g, h);
    i_expect_equal(resultS64.div, g / h);
    i_expect_equal(resultS64.mod, g % h);

#if 0
    f32 a = 100.0f;
    f32 b =  28.4f;
    DivModF32 resultF32 = divmod(a, b);
    i_expect_almost_equal(resultF32.div, a / b);
    i_expect_almost_equal(resultF32.mod, modulus(a, b));
#endif

}
TEST_END(divmod)

#if 0
TEST_BEGIN(trigonometry)
{
    f32 angle = 0.0f;

}
TEST_END(trigonometry)
#endif
