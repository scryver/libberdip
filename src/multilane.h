union f32_4x
{
    __m128  m;
    __m128i i;
    f32     e[4];
    u32     u[4];
};

struct SinCos_4x
{
    f32_4x sin;
    f32_4x cos;
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
F32_4x(f32 f0, f32 f1, f32 f2, f32 f3)
{
    f32_4x result;
    result.m = _mm_setr_ps(f0, f1, f2, f3);
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
square(f32_4x f4)
{
    f32_4x result;
    result = f4 * f4;
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
modulus(f32_4x a, f32_4x b)
{
    // NOTE(michiel): returns a % b <=> a - floor(a/b) * b
    f32_4x result;
    result = a - (floor(a / b) * b);
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

internal SinCos_4x
sincos_f32_4x_approx_small(f32_4x angles)
{
    // NOTE(michiel): Valid for angles between -0.5 and 0.5
    SinCos_4x result;

    f64 piSqOver4  = square(F64_PI) / 4.0;
    f64 pi4Over16  = square(piSqOver4);
    f64 pi6Over64  = pi4Over16 * piSqOver4;
    f64 pi8Over256 = square(pi4Over16);
    f64 cos_c0 = -piSqOver4 / (2.0 * 1.0);
    f64 cos_c1 = pi4Over16 / (4.0 * 3.0 * 2.0 * 1.0);
    f64 cos_c2 = -pi6Over64 / (6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f64 cos_c3 = pi8Over256 / (8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f64 sin_c0 = -piSqOver4 / (3.0 * 2.0 * 1.0);
    f64 sin_c1 = pi4Over16 / (5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f64 sin_c2 = -pi6Over64 / (7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f64 sin_c3 = pi8Over256 / (9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);

    f32_4x one_4x = F32_4x(1.0f);

    f32_4x angSq_4x = square(angles);
    result.sin = F32_4x(sin_c3) * angSq_4x;
    result.cos = F32_4x(cos_c3) * angSq_4x;
    result.sin = (result.sin + F32_4x(sin_c2)) * angSq_4x;
    result.cos = (result.cos + F32_4x(cos_c2)) * angSq_4x;
    result.sin = (result.sin + F32_4x(sin_c1)) * angSq_4x;
    result.cos = (result.cos + F32_4x(cos_c1)) * angSq_4x;
    result.sin = (result.sin + F32_4x(sin_c0)) * angSq_4x;
    result.cos = (result.cos + F32_4x(cos_c0)) * angSq_4x;
    result.sin = (result.sin + one_4x) * angles;
    result.cos = (result.cos + one_4x);
    result.sin = result.sin * F32_4x(0.5f * F32_PI);

    return result;
}

internal f32_4x
cos_f32_4x(f32_4x angles)
{
    f32_4x zero_4x  = zero_f32_4x();
    f32_4x eight_4x = F32_4x(0.125f);
    f32_4x half_4x  = F32_4x(0.5f);
    f32_4x one_4x   = F32_4x(1.0f);
    f32_4x two_4x   = F32_4x(2.0f);
    f32_4x four_4x  = F32_4x(4.0f);

    angles = angles + eight_4x;
    angles = modulus(angles, one_4x);
    angles = angles - eight_4x;
    angles = absolute(angles * four_4x);

    f32_4x quadrants = zero_f32_4x();

    f32_4x angleMask = angles > half_4x;
    angleMask = angleMask & one_4x;
    angles = (angles - angleMask);
    quadrants = (quadrants + angleMask);

    angleMask = angles > half_4x;
    angleMask = angleMask & one_4x;
    angles = (angles - angleMask);
    quadrants = (quadrants + angleMask);

    angleMask = angles > half_4x;
    angleMask = angleMask & one_4x;
    angles = (angles - angleMask);
    quadrants = (quadrants + angleMask);

    f32_4x do_cos_mask = quadrants == zero_4x;
    do_cos_mask = do_cos_mask | (quadrants == two_4x);

    f32_4x do_inv_mask = quadrants == one_4x;
    do_inv_mask = do_inv_mask | (quadrants == two_4x);

    SinCos_4x sincos = sincos_f32_4x_approx_small(angles);
    f32_4x result  = select(sincos.sin, do_cos_mask, sincos.cos);
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
