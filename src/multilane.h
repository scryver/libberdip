union f32_4x
{
    __m128  m;
    __m128i mi;
    f32     e[4];
    u32     u[4];
};

union f64_2x
{
    __m128  m;
    __m128i mi;
    f64     e[2];
    u64     u[2];
};

struct SinCos_4x
{
    f32_4x sin;
    f32_4x cos;
};

struct SinCos64_2x
{
    f64_2x sin;
    f64_2x cos;
};

internal f32_4x
zero_f32_4x(void)
{
    f32_4x result;
    result.m = _mm_setzero_ps();
    return result;
}

internal f32_4x
F32_4x(f32 f)
{
    f32_4x result;
    result.m = _mm_set1_ps(f);
    return result;
}

internal f32_4x
F32_4x(u32 u)
{
    f32_4x result;
    result.m = _mm_set1_ps(*(f32 *)&u);
    return result;
}

internal f32_4x
F32_4x(f32 f0, f32 f1, f32 f2, f32 f3)
{
    f32_4x result;
    result.m = _mm_setr_ps(f0, f1, f2, f3);
    return result;
}

internal f32_4x
S32_4x(s32 s)
{
    f32_4x result;
    result.m = _mm_set1_epi32(s);
    return result;
}

internal f32_4x
operator -(f32_4x f4)
{
    u32 signMask = (u32)(1 << 31);
    __m128 flipMask = _mm_set1_ps(*(f32 *)&signMask);
    f32_4x result;
    result.m = _mm_xor_ps(f4.m, flipMask);
    return result;
}

internal f32_4x
operator +(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_add_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator -(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_sub_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator *(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_mul_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator /(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_div_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator &(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_and_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator |(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_or_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator ^(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_xor_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator ==(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmpeq_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator !=(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmpneq_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator <(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmplt_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator <=(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmple_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator >(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmpgt_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator >=(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmpge_ps(a.m, b.m);
    return result;
}

internal f32_4x
and_not(f32_4x a, f32_4x b)
{
    // NOTE(michiel): Returns a & ~b
    f32_4x result;
    result.m = _mm_andnot_ps(b.m, a.m);
    return result;
}

internal f32_4x
square(f32_4x f4)
{
    f32_4x result;
    result = f4 * f4;
    return result;
}

internal f32_4x
square_root(f32_4x f4)
{
    f32_4x result;
    result.m = _mm_sqrt_ps(f4.m);
    return result;
}

internal f32_4x
absolute(f32_4x f4)
{
    u32 maskU32 = ~(u32)(1 << 31);
    __m128 mask = _mm_set1_ps(*(f32 *)&maskU32);

    f32_4x result;
    result.m = _mm_and_ps(f4.m, mask);
    return result;
}

internal f32_4x
floor(f32_4x f4)
{
    f32_4x result;
    result.m = _mm_floor_ps(f4.m);
    return result;
}

internal f32_4x
fraction(f32_4x a)
{
    f32_4x result;
    result = a - floor(a);
    return result;
}

internal f32_4x
modulus(f32_4x a, f32_4x b)
{
    // NOTE(michiel): returns a % b <=> a - floor(a/b) * b
    f32_4x result;
    result = a - (floor(a / b) * b);
    return result;
}

internal b32
any(f32_4x f4)
{
    b32 result;
    result = f4.u[0] || f4.u[1] || f4.u[2] || f4.u[3];
    return result;
}

internal b32
all(f32_4x f4)
{
    b32 result;
    result = f4.u[0] && f4.u[1] && f4.u[2] && f4.u[3];
    return result;
}

internal f32_4x
select(f32_4x op0, f32_4x mask, f32_4x op1)
{
    f32_4x result;
    result.m = _mm_or_ps(_mm_andnot_ps(mask.m, op0.m),
                         _mm_and_ps(mask.m, op1.m));
    return result;
}

//
// TODO(michiel): How to handle this s32_4x stuff
//

internal f32_4x
s32_4x_from_f32(f32_4x a)
{
    f32_4x result;
    result.mi = _mm_cvtps_epi32(a.m);
    return result;
}

internal f32_4x
f32_4x_from_s32(f32_4x a)
{
    f32_4x result;
    result.m = _mm_cvtepi32_ps(a.mi);
    return result;
}

internal f32_4x
s32_4x_and(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_and_si128(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_or(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_or_si128(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_add(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_add_epi32(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_sub(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_sub_epi32(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_equal(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_cmpeq_epi32(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_greater(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_cmpgt_epi32(a.mi, b.mi);
    return result;
}

//
// End of s32_4x stuff
//

alignas(16) global f32_4x gSinCosCoefs_4x[] =
{
    // NOTE(michiel): Cos/Sin coefficients interleaved
    {
        -1.0 / (2.0 * 1.0),
        -1.0 / (2.0 * 1.0),
        -1.0 / (2.0 * 1.0),
        -1.0 / (2.0 * 1.0)
    },
    {
        -1.0 / (3.0 * 2.0 * 1.0),
        -1.0 / (3.0 * 2.0 * 1.0),
        -1.0 / (3.0 * 2.0 * 1.0),
        -1.0 / (3.0 * 2.0 * 1.0)
    },
    {
        1.0 / (4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (4.0 * 3.0 * 2.0 * 1.0)
    },
    {
        1.0 / (5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (5.0 * 4.0 * 3.0 * 2.0 * 1.0)
    },
    {
        -1.0 / (6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        -1.0 / (6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        -1.0 / (6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        -1.0 / (6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0)
    },
    {
        -1.0 / (7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        -1.0 / (7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        -1.0 / (7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        -1.0 / (7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0)
    },
    {
        1.0 / (8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0)
    },
    {
        1.0 / (9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0),
        1.0 / (9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0)
    },
};

internal SinCos_4x
sincos_f32_4x_approx_small(f32_4x angles)
{
    // NOTE(michiel): Valid for angles between -0.5 and 0.5
    SinCos_4x result;

#if 0
    f64 piSqOver4  = square(F64_PI) / 4.0;
    f64 pi4Over16  = square(piSqOver4);
    f64 pi6Over64  = pi4Over16 * piSqOver4;
    //f64 pi8Over256 = square(pi4Over16);
    f32 cos_c0 = -piSqOver4 / (2.0 * 1.0);
    f32 cos_c1 = pi4Over16 / (4.0 * 3.0 * 2.0 * 1.0);
    f32 cos_c2 = -pi6Over64 / (6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    //f32 cos_c3 = pi8Over256 / (8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f32 sin_c0 = -piSqOver4 / (3.0 * 2.0 * 1.0);
    f32 sin_c1 = pi4Over16 / (5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f32 sin_c2 = -pi6Over64 / (7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    //f32 sin_c3 = pi8Over256 / (9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);

    f32_4x one_4x = F32_4x(1.0f);

    f32_4x angSq_4x = square(angles);
    //result.sin = F32_4x(sin_c3) * angSq_4x;
    //result.cos = F32_4x(cos_c3) * angSq_4x;
    //result.sin = (result.sin + F32_4x(sin_c2)) * angSq_4x;
    //result.cos = (result.cos + F32_4x(cos_c2)) * angSq_4x;
    result.sin = F32_4x(sin_c2) * angSq_4x;
    result.cos = F32_4x(cos_c2) * angSq_4x;
    result.sin = (result.sin + F32_4x(sin_c1)) * angSq_4x;
    result.cos = (result.cos + F32_4x(cos_c1)) * angSq_4x;
    result.sin = (result.sin + F32_4x(sin_c0)) * angSq_4x;
    result.cos = (result.cos + F32_4x(cos_c0)) * angSq_4x;
    result.sin = (result.sin + one_4x) * angles;
    result.cos = (result.cos + one_4x);
    result.sin = result.sin * F32_4x(0.5f * F32_PI);
#else

    f32_4x *coefPtr = gSinCosCoefs_4x;
    //f32 cos_c0 = -1.0 / (2.0 * 1.0);
    //f32 cos_c1 = 1.0 / (4.0 * 3.0 * 2.0 * 1.0);
    //f32 cos_c2 = -1.0 / (6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    //f32 cos_c3 = 1.0 / (8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    //f32 sin_c0 = -1.0 / (3.0 * 2.0 * 1.0);
    //f32 sin_c1 = 1.0 / (5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    //f32 sin_c2 = -1.0 / (7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    //f32 sin_c3 = 1.0 / (9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);

    f32_4x one_4x = F32_4x(1.0f);
    f32_4x halfPi_4x = F32_4x(0.5f * F32_PI);

    f32_4x piAngles_4x = halfPi_4x * angles;
    f32_4x angSq_4x = square(piAngles_4x);
    result.cos = *coefPtr++ * angSq_4x;
    result.sin = *coefPtr++ * angSq_4x;
    //result.cos = (result.cos + *coefPtr++) * angSq_4x;
    //result.sin = (result.sin + *coefPtr++) * angSq_4x;
    result.cos = (result.cos + *coefPtr++) * angSq_4x;
    result.sin = (result.sin + *coefPtr++) * angSq_4x;
    result.cos = (result.cos + *coefPtr++) * angSq_4x;
    result.sin = (result.sin + *coefPtr++) * angSq_4x;
    result.cos = (result.cos + one_4x);
    result.sin = (result.sin + one_4x) * piAngles_4x;
#endif

    return result;
}

#if 1
internal f32_4x
cos_f32_4x(f32_4x angles)
{
    f32_4x eight_4x = F32_4x(0.125f);
    f32_4x one_4x   = F32_4x(1.0f);

    angles = angles + eight_4x;
    //angles = modulus(angles, one_4x);
    angles = fraction(angles);
    angles = angles - eight_4x;
    angles = absolute(angles * F32_4x(4.0f));

    f32_4x quadrant4 = angles > F32_4x(2.5f);
    f32_4x quadrant3 = angles > F32_4x(1.5f);
    f32_4x quadrant2 = angles > F32_4x(0.5f);

    quadrant2 = and_not(quadrant2, quadrant3);
    quadrant3 = and_not(quadrant3, quadrant4);

    f32_4x quadrants = (F32_4x(3.0f) & quadrant4) | (F32_4x(2.0f) & quadrant3) | (one_4x & quadrant2);
    angles = angles - quadrants;

    f32_4x do_cos_mask = quadrant3 | (quadrants == zero_f32_4x());
    f32_4x do_inv_mask = quadrant2 | quadrant3;

    SinCos_4x sincos = sincos_f32_4x_approx_small(angles);
    f32_4x result = select(sincos.sin, do_cos_mask, sincos.cos);
    result = select(result, do_inv_mask, -result);

    return result;
}

internal f32_4x
sin_f32_4x(f32_4x angles)
{
    f32_4x quarter_4x = F32_4x(0.25f);
    f32_4x result = cos_f32_4x(angles - quarter_4x);
    return result;
}
#else
internal f32_4x
cos_f32_4x(f32_4x angles)
{
    f32_4x result;
    result.e[0] = __builtin_cosf(F32_TAU * angles.e[0]);
    result.e[1] = __builtin_cosf(F32_TAU * angles.e[1]);
    result.e[2] = __builtin_cosf(F32_TAU * angles.e[2]);
    result.e[3] = __builtin_cosf(F32_TAU * angles.e[3]);
    return result;
}

internal f32_4x
sin_f32_4x(f32_4x angles)
{
    f32_4x result;
    result.e[0] = __builtin_sinf(F32_TAU * angles.e[0]);
    result.e[1] = __builtin_sinf(F32_TAU * angles.e[1]);
    result.e[2] = __builtin_sinf(F32_TAU * angles.e[2]);
    result.e[3] = __builtin_sinf(F32_TAU * angles.e[3]);
    return result;
}
#endif

internal f32_4x
tan_f32_4x(f32_4x angles)
{
    f32_4x one_4x   = F32_4x(1.0f);
    angles = angles * F32_4x(2.0f);

#if 0
    // TODO(michiel): With sincos_f32_4x_approx_small
    f32_4x eight_4x = F32_4x(0.125f);

    angles = angles + eight_4x;
    angles = modulus(angles, one_4x);
    angles = angles - eight_4x;
    angles = absolute(angles * F32_4x(4.0f));

    f32_4x quadrant4 = angles > F32_4x(2.5f);
    f32_4x quadrant3 = angles > F32_4x(1.5f);
    f32_4x quadrant2 = angles > F32_4x(0.5f);

    quadrant2 = and_not(quadrant2, quadrant3);
    quadrant3 = and_not(quadrant3, quadrant4);

    f32_4x quadrants = (F32_4x(3.0f) & quadrant4) | (F32_4x(2.0f) & quadrant3) | (one_4x & quadrant2);
    angles = angles - quadrants;

    SinCos_4x sincos = sincos_f32_4x_approx_small(angles);
    f32_4x do_cos_mask = quadrant3 | (quadrants == zero_f32_4x());
    f32_4x do_inv_cos_mask = quadrant2 | quadrant3;
    f32_4x do_inv_sin_mask = quadrant3 | quadrant4;

    f32_4x cosine = select(sincos.sin, do_cos_mask, sincos.cos);
    cosine = select(cosine, do_inv_cos_mask, -cosine);
    f32_4x sine = select(sincos.cos, do_cos_mask, sincos.sin);
    sine = select(sine, do_inv_sin_mask, -sine);
#else
    f32_4x cosine = cos_f32_4x(angles);
    f32_4x sine = sin_f32_4x(angles);
#endif

    f32_4x result = sine / (one_4x + cosine);
    return result;
}

internal f32_4x
exp_f32_4x(f32_4x f4)
{
    // NOTE(michiel): Change the 12 (and the 12 multiplies) to tune the approximation
    f32_4x result = F32_4x(1.0f);
    f32_4x divisor = F32_4x(1.0f / (f32)(1 << 12));
    result = result + f4 * divisor;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    return result;
}
