struct RandomSeriesPCG_4x
{
    f32_4x state;
    f32_4x selector;
};

internal RandomSeriesPCG_4x
random_seed_pcg(f32_4x state, f32_4x selector)
{
    RandomSeriesPCG_4x result = {};

    result.state = state;
    result.selector.mi = _mm_slli_epi64(selector.mi, 1);
    result.selector = result.selector | S32_4x(1, 0, 1, 0);

    return result;
}

internal f32_4x
random_next_u32_4x(RandomSeriesPCG_4x *series)
{
    f32_4x multiplier = {};
    multiplier.u[0] = 0x4c957f2d;
    multiplier.u[1] = 0x5851f42d;
    multiplier.u[2] = 0x4c957f2d;
    multiplier.u[3] = 0x5851f42d;

    f32_4x mask = S32_4x(-1, 0, -1, 0);

    f32_4x state0 = series->state;
    state0 = mul_s64_2x(state0, multiplier);
    state0.mi = _mm_add_epi64(state0.mi, series->selector.mi);

    f32_4x state1 = state0;
    state1 = mul_s64_2x(state1, multiplier);
    state1.mi = _mm_add_epi64(state1.mi, series->selector.mi);

    series->state = state1;

    f32_4x preRotate0;
    preRotate0.mi = _mm_srli_epi64(state0.mi, 18);
    preRotate0.mi = _mm_xor_si128(preRotate0.mi, state0.mi);
    preRotate0.mi = _mm_srli_epi64(preRotate0.mi, 27);
    preRotate0.mi = _mm_and_si128(preRotate0.mi, mask.mi);

    f32_4x preRotate1;
    preRotate1.mi = _mm_srli_epi64(state1.mi, 18);
    preRotate1.mi = _mm_xor_si128(preRotate1.mi, state1.mi);
    preRotate1.mi = _mm_srli_epi64(preRotate1.mi, 27);
    preRotate1.mi = _mm_and_si128(preRotate1.mi, mask.mi);

    preRotate0.mi = _mm_shuffle_epi32(preRotate0.mi, MULTILANE_SHUFFLE_MASK(0, 0, 2, 2));
    preRotate1.mi = _mm_shuffle_epi32(preRotate1.mi, MULTILANE_SHUFFLE_MASK(0, 0, 2, 2));

    f32_4x rotate0;
    rotate0.mi = _mm_srli_epi64(state0.mi, 59);
    f32_4x rotate1;
    rotate1.mi = _mm_srli_epi64(state1.mi, 59);
    rotate0.mi = _mm_srl_epi64(preRotate0.mi, rotate0.mi);
    rotate1.mi = _mm_srl_epi64(preRotate1.mi, rotate1.mi);

    rotate0.mi = _mm_and_si128(rotate0.mi, mask.mi);
    rotate1.mi = _mm_slli_epi64(rotate1.mi, 32);

    f32_4x result;
    result.mi = _mm_or_si128(rotate0.mi, rotate1.mi);
    return result;
}

internal f32_4x
random_unilateral_4x(RandomSeriesPCG_4x *series)
{
    f32_4x oneOverMax = F32_4x(1.0f / (f32)U32_MAX);
    f32_4x result = random_next_u32_4x(series) / oneOverMax;
    return result;
}

internal f32_4x
random_bilateral_4x(RandomSeriesPCG_4x *series)
{
    f32_4x one = F32_4x(1.0f);
    f32_4x two = F32_4x(2.0f);
    f32_4x result = random_unilateral_4x(series) * two - one;
    return result;
}
