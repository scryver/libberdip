
internal f32
cos_f32_approx8_small(f32 angle)
{
    i_expect(-(0.5f + 0.000001f) <= angle);
    i_expect(angle <= (0.5f + 0.000001f));
    // cos(x) = 1 - (x^2/2!) + (x^4/4!) - (x^6/6!) + (x^8/8!)
    // cos(x) = 1 - x^2 (1/2! + x^2 (1/4! - x^2 (1/6! + x^2/8!)))

    // TODO(michiel): How to improve accuracy
    f32 c0 = -1.2337005501361697490381175157381221652030944824f; // NOTE(michiel): (pi^2 /   4) / 2!
    f32 c1 = 0.25366950790104797475166265030566137284040451050f; // NOTE(michiel): (pi^4 /  16) / 4!
    f32 c2 = -0.0208634807633529574533159944849103339947760105f; // NOTE(michiel): (pi^6 /  64) / 6!
    f32 c3 = 0.00091926027483942625989632446703581081237643957f; // NOTE(michiel): (pi^8 / 256) / 8!

    f32 angSq = square(angle);
    //f32 result = 1.0f + angSq * (c0 + angSq * (c1 + angSq * (c2 + c3 * angSq)));

    f32 angQd = square(square(angle));
    f32 a = angQd * (c1 + angQd * c3);
    f32 b = angSq * (c0 + angQd * c2);
    f32 result = 1.0f + a + b;
    return result;
}

internal f32
sin_f32_approx7_small(f32 angle)
{
    i_expect(-(0.5f + 0.000001f) <= angle);
    i_expect(angle <= (0.5f + 0.000001f));
    // sin(x) = x - (x^3/3!) + (x^5/5!) - (x^7/7!) + (x^9/9!)
    // sin(x) = x (1 - x^2 (1/3! + x^2 (1/5! - x^2 (1/7! + x^2/9!)))

    // TODO(michiel): How to improve accuracy
    f32 c0 = -0.4112335167120566015164229156653163954615592956f; // NOTE(michiel): (pi^2 /   4) / 3!
    f32 c1 = 0.05073390158020959356255374927968659903854131698f; // NOTE(michiel): (pi^4 /  16) / 5!
    f32 c2 = -0.0029804972519075654743825332104734116001054644f; // NOTE(michiel): (pi^6 /  64) / 7!
    f32 c3 = 0.00010214003053771402134818763185464263187895994f; // NOTE(michiel): (pi^8 / 256) / 9!

    f32 angSq = square(angle);
    //f32 result = 0.5f * F32_PI * angle * (1.0f + angSq * (c0 + angSq * (c1 + angSq * (c2 + c3 * angSq))));

    f32 angQd = square(square(angle));
    f32 a = angQd * (c1 + angQd * c3);
    f32 b = angSq * (c0 + angQd * c2);
    f32 result = 0.5f * F32_PI * angle * (1.0f + a + b);
    return result;
}

#define SINCOS_F64_PRECISION 6

internal f64
cos_f64_approx8_small(f64 angle)
{
    i_expect(-(0.5 + 0.00000001) <= angle);
    i_expect(angle <= (0.5 + 0.00000001));

    // TODO(michiel): How to improve accuracy
    f64 piSqOver4  = square(F64_PI) / 4.0;
#if SINCOS_F64_PRECISION > 1
    f64 pi4Over16  = square(piSqOver4);
#if SINCOS_F64_PRECISION > 2
    f64 pi6Over64  = pi4Over16 * piSqOver4;
#if SINCOS_F64_PRECISION > 3
    f64 pi8Over256 = square(pi4Over16);
#if SINCOS_F64_PRECISION > 4
    f64 pi10Over1024 = pi8Over256 * piSqOver4;
#if SINCOS_F64_PRECISION > 5
    f64 pi12Over4096 = pi8Over256 * pi4Over16;
#if SINCOS_F64_PRECISION > 6
    f64 pi14Over16384 = pi8Over256 * pi6Over64;
#endif
#endif
#endif
#endif
#endif
#endif

    f64 c0 = -piSqOver4 / (2.0 * 1.0);
#if SINCOS_F64_PRECISION > 1
    f64 c1 = pi4Over16 / (4.0 * 3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 2
    f64 c2 = -pi6Over64 / (6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 3
    f64 c3 = pi8Over256 / (8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 4
    f64 c4 = -pi10Over1024 / (10.0 * 9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 5
    f64 c5 = pi12Over4096 / (12.0 * 11.0 * 10.0 * 9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 6
    f64 c6 = -pi14Over16384 / (14.0 * 13.0 * 12.0 * 11.0 * 10.0 * 9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#endif
#endif
#endif
#endif
#endif
#endif

    f64 angSq = square(angle);
    //f64 result = 1.0 + angSq * (c0 + angSq * (c1 + angSq * (c2 + c3 * angSq)));
#if SINCOS_F64_PRECISION > 6
    f64 result = c6 * angSq + c5;
    result = result * angSq + c4;
    result = result * angSq + c3;
    result = result * angSq + c2;
    result = result * angSq + c1;
    result = result * angSq + c0;
#elif SINCOS_F64_PRECISION > 5
    f64 result = c5 * angSq + c4;
    result = result * angSq + c3;
    result = result * angSq + c2;
    result = result * angSq + c1;
    result = result * angSq + c0;
#elif SINCOS_F64_PRECISION > 4
    f64 result = c4 * angSq + c3;
    result = result * angSq + c2;
    result = result * angSq + c1;
    result = result * angSq + c0;
#elif SINCOS_F64_PRECISION > 3
    f64 result = c3 * angSq + c2;
    result = result * angSq + c1;
    result = result * angSq + c0;
#elif SINCOS_F64_PRECISION > 2
    f64 result = c2 * angSq + c1;
    result = result * angSq + c0;
#elif SINCOS_F64_PRECISION > 1
    f64 result = c1 * angSq + c0;
#else
    f64 result = c0;
#endif
    //f64 result = c3 * angSq + c2;
    //result = result * angSq + c1;
    //result = result * angSq + c0;
    result = result * angSq + 1.0;
    return result;
}

internal f64
sin_f64_approx7_small(f64 angle)
{
    i_expect(-(0.5 + 0.00000001) <= angle);
    i_expect(angle <= (0.5 + 0.00000001));

    // TODO(michiel): How to improve accuracy
    f64 piSqOver4  = square(F64_PI) / 4.0;
#if SINCOS_F64_PRECISION > 1
    f64 pi4Over16  = square(piSqOver4);
#if SINCOS_F64_PRECISION > 2
    f64 pi6Over64  = pi4Over16 * piSqOver4;
#if SINCOS_F64_PRECISION > 3
    f64 pi8Over256 = square(pi4Over16);
#if SINCOS_F64_PRECISION > 4
    f64 pi10Over1024 = pi8Over256 * piSqOver4;
#if SINCOS_F64_PRECISION > 5
    f64 pi12Over4096 = pi8Over256 * pi4Over16;
#if SINCOS_F64_PRECISION > 6
    f64 pi14Over16384 = pi8Over256 * pi6Over64;
#endif
#endif
#endif
#endif
#endif
#endif

    f64 c0 = -piSqOver4 / (3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 1
    f64 c1 = pi4Over16 / (5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 2
    f64 c2 = -pi6Over64 / (7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 3
    f64 c3 = pi8Over256 / (9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 4
    f64 c4 = -pi10Over1024 / (11.0 * 10.0 * 9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 5
    f64 c5 = pi12Over4096 / (13.0 * 12.0 * 11.0 * 10.0 * 9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#if SINCOS_F64_PRECISION > 6
    f64 c6 = -pi14Over16384 / (15.0 * 14.0 * 13.0 * 12.0 * 11.0 * 10.0 * 9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
#endif
#endif
#endif
#endif
#endif
#endif

    f64 angSq = square(angle);
    //f64 result = 0.5 * F64_PI * angle * (1.0 + angSq * (c0 + angSq * (c1 + angSq * (c2 + c3 * angSq))));
#if SINCOS_F64_PRECISION > 6
    f64 result = c6 * angSq + c5;
    result = result * angSq + c4;
    result = result * angSq + c3;
    result = result * angSq + c2;
    result = result * angSq + c1;
    result = result * angSq + c0;
#elif SINCOS_F64_PRECISION > 5
    f64 result = c5 * angSq + c4;
    result = result * angSq + c3;
    result = result * angSq + c2;
    result = result * angSq + c1;
    result = result * angSq + c0;
#elif SINCOS_F64_PRECISION > 4
    f64 result = c4 * angSq + c3;
    result = result * angSq + c2;
    result = result * angSq + c1;
    result = result * angSq + c0;
#elif SINCOS_F64_PRECISION > 3
    f64 result = c3 * angSq + c2;
    result = result * angSq + c1;
    result = result * angSq + c0;
#elif SINCOS_F64_PRECISION > 2
    f64 result = c2 * angSq + c1;
    result = result * angSq + c0;
#elif SINCOS_F64_PRECISION > 1
    f64 result = c1 * angSq + c0;
#else
    f64 result = c0;
#endif
    //f64 result = c3 * angSq + c2;
    //result = result * angSq + c1;
    //result = result * angSq + c0;
    result = result * angSq + 1.0;
    result = result * angle * F64_PI * 0.5;
    return result;
}

internal f32
cos_f32(f32 angle)
{
    f32 result = 0;

    if (!(is_nan(angle) || is_infinite(angle)))
    {
        angle += 0.125f;              // Map [0, 1] to [0.125, 1.125]
        angle = modulus01(angle);     // Mod to [0, 1]
        angle -= 0.125f;              // Map [0, 1] to [-0.125, 0.875]
        angle = absolute(angle * 4.0f); // Map to [-0.5, 3.5] and fold the negative part

        if (angle <= 0.5f)
        {
            result = cos_f32_approx8_small(angle);
        }
        else if (angle <= 1.5f)
        {
            result = -sin_f32_approx7_small(angle - 1.0f);
        }
        else if (angle <= 2.5f)
        {
            result = -cos_f32_approx8_small(angle - 2.0f);
        }
        else
        {
            i_expect(angle <= 3.5f);
            result = sin_f32_approx7_small(angle - 3.0f);
        }
    }
    else
    {
        result = F32_NAN;
    }

    return result;
}

internal f32
sin_f32(f32 angle)
{
    f32 result = cos_f32(angle - 0.25f);
    return result;
}

internal f64
cos_f64(f64 angle)
{
    f64 result = 0;

    if (!(is_nan(angle) || is_infinite(angle)))
    {
        angle += 0.125;              // Map [0, 4] to [0.5, 4.5]
        angle = modulus01(angle);     // Mod to [0, 4]
        angle -= 0.125;              // Map [0, 4] to [-0.5, 3.5]
        angle = absolute(angle * 4.0);

        if (angle <= 0.5)
        {
            result = cos_f64_approx8_small(angle);
        }
        else if (angle <= 1.5)
        {
            result = -sin_f64_approx7_small(angle - 1.0);
        }
        else if (angle <= 2.5)
        {
            result = -cos_f64_approx8_small(angle - 2.0);
        }
        else
        {
            i_expect(angle <= 3.5);
            result = sin_f64_approx7_small(angle - 3.0);
        }
    }
    else
    {
        result = F64_NAN;
    }

    return result;
}

internal f64
sin_f64(f64 angle)
{
    f64 result = cos_f64(angle - 0.25);
    return result;
}

internal f32
cos_pi(f32 radians)
{
    f32 oneOverTau = 1.0f / F32_TAU;
    f32 result = cos_f32(radians * oneOverTau);
    return result;
}

internal f64
cos_pi(f64 radians)
{
    f64 oneOverTau = 1.0 / F64_TAU;
    f64 result = cos_f64(radians * oneOverTau);
    return result;
}

internal f32
sin_pi(f32 radians)
{
    f32 oneOverTau = 1.0f / F32_TAU;
    f32 result = sin_f32(radians * oneOverTau);
    return result;
}

internal f64
sin_pi(f64 radians)
{
    f64 oneOverTau = 1.0 / F64_TAU;
    f64 result = sin_f64(radians * oneOverTau);
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

internal f64
tan_pi(f64 angle)
{
    f64 result;
    result = sin_pi(2.0 * angle) / (1.0 + cos_pi(2.0 * angle));
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

internal f32
atan_pi(f32 angle)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)atanf(angle);
#elif __has_builtin(__builtin_atanf)
    result = (f32)__builtin_atanf(angle);
#else
#error No atanf builtin!
#endif
    return result;
}

internal f32
atan2_pi(f32 y, f32 x)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)atan2f(y, x);
#elif __has_builtin(__builtin_atan2f)
    result = (f32)__builtin_atan2f(y, x);
#else
#error No atan2f builtin!
#endif
    return result;
}

internal f32
sinh(f32 x)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)sinhf(x);
#elif __has_builtin(__builtin_sinhf)
    result = (f32)__builtin_sinhf(x);
#else
#error No sinhf builtin!
#endif
    return result;
}

internal f32
cosh(f32 x)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)coshf(x);
#elif __has_builtin(__builtin_coshf)
    result = (f32)__builtin_coshf(x);
#else
#error No coshf builtin!
#endif
    return result;
}

internal f64
sinh(f64 x)
{
    f64 result;
#if !NO_INTRINSICS && __has_builtin(__builtin_sinh)
    result = __builtin_sinh(x);
#else
#error No sinh builtin!
#endif
    return result;
}

internal f64
cosh(f64 x)
{
    f64 result;
#if !NO_INTRINSICS && __has_builtin(__builtin_cosh)
    result = (f64)__builtin_cosh(x);
#else
#error No cosh builtin!
#endif
    return result;
}
