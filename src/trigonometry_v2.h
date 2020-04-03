// NOTE(michiel): Stripped down cephes sin/cos/atan version. It was just faster and more accurate than our own v1.

/* These are for a 24-bit significand: */
#define F32_PI_OVER_4_PREC_1 0.78515625f
#define F32_PI_OVER_4_PREC_2 2.4187564849853515625e-4f
#define F32_PI_OVER_4_PREC_3 3.77489497744594108e-8f

#define F32_SIN_COEF_0       1.6666654611e-1f
#define F32_SIN_COEF_1       8.3321608736e-3f
#define F32_SIN_COEF_2       1.9515295891e-4f

#define F32_COS_COEF_0       0.5f
#define F32_COS_COEF_1       4.166664568298827e-2f
#define F32_COS_COEF_2       1.388731625493765e-3f
#define F32_COS_COEF_3       2.443315711809948e-5f

#define F32_ATAN_COEF_0      3.33329491539e-1f
#define F32_ATAN_COEF_1      1.99777106478e-1f
#define F32_ATAN_COEF_2      1.38776856032e-1f
#define F32_ATAN_COEF_3      8.05374449538e-2f

internal v2
sincos_pi(f32 angle)
{
    // NOTE(michiel): x == sin, y == cos
    // TODO(michiel): Maybe flip? For cartesian/polar conversion
    f32 sinSign = 1.0f;
    f32 cosSign = 1.0f;
    if (angle < 0)
	{
        sinSign = -sinSign;
        angle = -angle;
	}

    u32 j = (u32)(F32_FOUR_OVER_PI * angle); /* integer part of x/(PI/4) */
    f32 y = (f32)j;
    /* map zeros to origin */
    if (j & 0x1)
	{
        ++j;
        y += 1.0f;
	}

    j &= 0x7; /* octant modulo 360 degrees */
    /* reflect in x axis */
    if (j > 3)
	{
        sinSign = -sinSign;
        cosSign = -cosSign;
        j -= 4;
	}

    if (j > 1)
    {
        cosSign = -cosSign;
    }

    /* Extended precision modular arithmetic */
    f32 x = ((angle - y * F32_PI_OVER_4_PREC_1) - y * F32_PI_OVER_4_PREC_2) - y * F32_PI_OVER_4_PREC_3;

    f32 xSquare = x * x;
    f32 xQuad = xSquare * xSquare;

    /* measured relative error in +/- pi/4 is 7.8e-8 */
    // 1 - 1/2! x^2 + 1/4! x^4 - 1/6! x^6 + 1/8! x^8
    // 1 - A + B | A = x^2 (1/2 + 1/6! x^4), B = x^4 (1/4! + 1/8! x^4)
    f32 cosA = xSquare * (F32_COS_COEF_0 + xQuad * F32_COS_COEF_2);
    f32 cosB = xQuad   * (F32_COS_COEF_1 + xQuad * F32_COS_COEF_3);
    f32 cosApprox = 1.0f - cosA + cosB;

    /* Theoretical relative error = 3.8e-9 in [-pi/4, +pi/4] */
    // x - 1/3! x^3 + 1/5! x^5 - 1/7! x^7 = x (1 - 1/3! x^2 + 1/5! x^4 - 1/7! x^6)
    // x (1 - A + B) | A = x^2 (1/3! + 1/7! x^4), B = x^4 (1/5!)
    f32 sinA = xSquare * (F32_SIN_COEF_0 + xQuad * F32_SIN_COEF_2);
    f32 sinB = xQuad   * (F32_SIN_COEF_1);
    f32 sinApprox = x * (1.0f - sinA + sinB);

    v2 result = {sinApprox, cosApprox};
    if ((j == 1) || (j == 2))
	{
        result.x = cosApprox;
        result.y = sinApprox;
	}

    result.x = result.x * sinSign;
    result.y = result.y * cosSign;

    return result;
}

internal f32
sin_pi(f32 angle)
{
    // NOTE(michiel): Just a wrapper (it's not slower than seperate functions)
    v2 result = sincos_pi(angle);
    return result.x;
}

internal f32
cos_pi(f32 angle)
{
    // NOTE(michiel): Just a wrapper (it's not slower than seperate functions)
    v2 result = sincos_pi(angle);
    return result.y;
}

internal v2
sincos_f32(f32 angle)
{
    // NOTE(michiel): angle in 0 - 1 range
    return sincos_pi(angle * F32_TAU);
}

internal f32
sin_f32(f32 angle)
{
    // NOTE(michiel): angle in 0 - 1 range
    v2 result = sincos_f32(angle);
    return result.x;
}

internal f32
cos_f32(f32 angle)
{
    // NOTE(michiel): angle in 0 - 1 range
    v2 result = sincos_f32(angle);
    return result.y;
}

internal f32
atan_pi(f32 x)
{
    f32 sign = 1.0f;
    if (x < 0)
    {
        sign = -sign;
        x = -x;
    }

    f32 result = 0.0f;
    /* range reduction */
    if (x > 2.414213562373095f) /* tan 3pi/8 */
	{
        result = F32_PI_OVER_2;
        x = -1.0f / x;
	}
    else if (x > 0.4142135623730950f) /* tan pi/8 */
	{
        result = F32_PI_OVER_4;
        x = (x - 1.0f) / (x + 1.0f);
	}

    f32 xSquare = x * x;
    f32 xQuad = xSquare * xSquare;

    // NOTE(michiel):
    // x (1 - a0 x^2 + a1 x^4 - a2 x^6 + a3 x^8)
    // x (1 - A + B) | A = x^2(a0 + x^4 a2), B = x^4(a1 + x^4 a3)
    f32 a = xSquare * (F32_ATAN_COEF_0 + xQuad * F32_ATAN_COEF_2);
    f32 b = xQuad   * (F32_ATAN_COEF_1 + xQuad * F32_ATAN_COEF_3);
    result += x * (1.0f - a + b);
    result *= sign;

    return result;
}

internal f32
atan2_pi(f32 y, f32 x)
{
    b32 xNegative = x < 0;
    b32 yNegative = y < 0;

    f32 result = 0.0f;
    if (x == 0.0f)
	{
        if (yNegative)
		{
            result = -F32_PI_OVER_2;
		}
        else if (y != 0.0f)
        {
            result = F32_PI_OVER_2;
        }
	}
    else
    {
        if (y == 0.0f)
        {
            if (xNegative)
            {
                result = F32_PI;
            }
        }
        else
        {
            if (xNegative)
            {
                result = yNegative ? -F32_PI : F32_PI;
            }

            f32 z = atan_pi(y / x);
            result = result + z;
        }
    }

    return result;
}


internal f32
tan_pi(f32 angle)
{
    f32 result;
    result = sin_pi(2.0f * angle) / (1.0f + cos_pi(2.0f * angle));

#if 0
#if NO_INTRINSICS
    result = (f32)tanf(angle);
#elif __has_builtin(__builtin_tanf)
    result = (f32)__builtin_tanf(angle);
#else
#error No tan2f builtin!
#endif
#endif

    return result;
}

internal f32
asin_pi(f32 angle)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)asinf(angle);
#elif __has_builtin(__builtin_asinf)
    result = (f32)__builtin_asinf(angle);
#else
#error No asinf builtin!
#endif
    return result;
}

internal f32
acos_pi(f32 angle)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)acosf(angle);
#elif __has_builtin(__builtin_acosf)
    result = (f32)__builtin_acosf(angle);
#else
#error No acosf builtin!
#endif
    return result;
}

internal f64
sin_pi(f64 angle)
{
    return __builtin_sin(angle);
}

internal f64
cos_pi(f64 angle)
{
    return __builtin_cos(angle);
}

internal f64
atan2_pi(f64 y, f64 x)
{
    return __builtin_atan2(y, x);
}
