#if NO_INTRINSICS
// TODO(michiel): Remove dependency
#include <math.h>
#endif

internal f32
fast_exp_(f32 x, u32 approx)
{
    f32 result = 1.0f;
    result += x / (f32)(1 << approx);
    u32 approx4 = approx >> 2;
    u32 approxR = approx & 0x3;
    for (u32 guess = 0; guess < approx4; ++guess)
    {
        result *= result;
        result *= result;
        result *= result;
        result *= result;
    }
    for (u32 guess = 0; guess < approxR; ++guess)
    {
        result *= result;
    }
    return result;
}

internal f32
fast_exp(f32 x)
{
    return fast_exp_(x, 12);
}

internal u32
log2_up(u32 value)
{
    u32 bitPos = 0;
    while (((u64)1 << bitPos) <= value)
    {
        ++bitPos;
    }
    return bitPos;
}

internal u32
lerp(u32 min, f32 t, u32 max)
{
    u32 result = min;
    result += (u32)(((f32)max - (f32)min) * t);
    return result;
}

internal s32
lerp(s32 min, f32 t, s32 max)
{
    s32 result = min;
    result += (s32)((f32)(max - min) * t);
    return result;
}

internal f32
lerp(f32 min, f32 t, f32 max)
{
    f32 result = min;
    result += (max - min) * t;
    return result;
}

#if 0

internal inline f32
map(f32 value, f32 fromMin, f32 fromMax, f32 toMin, f32 toMax)
{
    f32 result;
    result = (value - fromMin) / (fromMax - fromMin);
    result = result * (toMax - toMin) + toMin;
    return result;
}

internal inline f64
map(f64 value, f64 fromMin, f64 fromMax, f64 toMin, f64 toMax)
{
    f64 result;
    result = (value - fromMin) / (fromMax - fromMin);
    result = result * (toMax - toMin) + toMin;
    return result;
}

internal inline v2
map(v2 value, v2 fromMin, v2 fromMax, v2 toMin, v2 toMax)
{
    v2 result;
    result.x = map(value.x, fromMin.x, fromMax.x, toMin.x, toMax.x);
    result.y = map(value.y, fromMin.y, fromMax.y, toMin.y, toMax.y);
    return result;
}

#endif

internal f32
square(f32 f)
{
    f32 result = f * f;
    return result;
}

internal f64
square(f64 f)
{
    f64 result = f * f;
    return result;
}

internal s32
absolute(s32 value)
{
    s32 result = (value & 0x80000000) ? -value : value;
    return result;
}

internal s64
absolute(s64 value)
{
    s64 result = (value & 0x8000000000000000) ? -value : value;
    return result;
}

internal f32
absolute(f32 value)
{
    u32 val32 = *(u32 *)&value;
    val32 &= ~F32_SIGN_MASK;
    f32 result = *(f32 *)&val32;
    return result;
}

internal f64
absolute(f64 value)
{
    u64 val64 = *(u64 *)&value;
    val64 &= ~F64_SIGN_MASK;
    f64 result = *(f64 *)&val64;
    return result;
}

internal s32
sign_of(s32 value)
{
    s32 result = (value >= 0) ? 1 : -1;
    return result;
}

internal s64
sign_of(s64 value)
{
    s64 result = (value >= 0) ? 1 : -1;
    return result;
}

internal f32
sign_of(f32 value)
{
    f32 result = (value >= 0.0f) ? 1.0f : -1.0f;
    return result;
}

internal f64
sign_of(f64 value)
{
    f64 result = (value >= 0.0) ? 1.0 : -1.0;
    return result;
}

internal f32
floor(f32 value)
{
    f32 result;
    result = (f32)(s32)(value - (value < 0.0f ? 0.9999999f : 0.0));
    return result;
}

internal f64
floor(f64 value)
{
    f64 result;
    result = (f64)(s64)(value + (value < 0.0 ? -0.999999999999999 : 0.0));
    return result;
}

internal f32
ceil(f32 value)
{
    f32 result;
    result = (f32)(s32)(value + (value < 0.0f ? 0.0f : 0.9999999f));
    return result;
}

internal f64
ceil(f64 value)
{
    f64 result;
    result = (f64)(s64)(value + (value < 0.0 ? 0.0 : 0.999999999999999));
    return result;
}

internal f32
round(f32 value)
{
    f32 result;
    result = (f32)(s32)(value + 0.5f * sign_of(value));
    return result;
}

internal f64
round(f64 value)
{
    f64 result;
    result = (f64)(s64)(value + 0.5 * sign_of(value));
    return result;
}

// TODO(michiel): Profile this vs compile implementation
internal DivModU32
divmod(u32 a, u32 b)
{
    DivModU32 result = {a, 0};
    asm ("div %4\n\t"
         : "=a"(result.div), "=d"(result.mod)
         : "a"(result.div), "d"(result.mod), "r"(b)
         : "cc");
    return result;
}

internal DivModS32
divmod(s32 a, s32 b)
{
    DivModS32 result = {a, sign_of(a)};
    asm ("idiv %4\n\t"
         : "=a"(result.div), "=d"(result.mod)
         : "a"(result.div), "d"(result.mod), "r"(b)
         : "cc");
    return result;
}

internal DivModU64
divmod(u64 a, u64 b)
{
    DivModU64 result = {a, 0};
    asm ("div %4\n\t"
         : "=a"(result.div), "=d"(result.mod)
         : "a"(result.div), "d"(result.mod), "r"(b)
         : "cc");
    return result;
}

internal DivModS64
divmod(s64 a, s64 b)
{
    DivModS64 result = {a, sign_of(a)};
    asm ("idiv %4\n\t"
         : "=a"(result.div), "=d"(result.mod)
         : "a"(result.div), "d"(result.mod), "r"(b)
         : "cc");
    return result;
}

internal f32
modulus01(f32 f)
{
    f32 result;
    result = f - floor(f);
    return result;
}

internal f64
modulus01(f64 f)
{
    f64 result;
    result = f - floor(f);
    return result;
}

internal f32
modulus(f32 x, f32 y)
{
    f32 result;
    result = x - floor(x / y) * y;
    return result;
}

internal f64
modulus(f64 x, f64 y)
{
    f64 result;
    result = x - floor(x / y) * y;
    return result;
}

internal f32
exp(f32 f)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)expf(f);
#elif __has_builtin(__builtin_expf)
    result = (f32)__builtin_expf(f);
#else
#error No expf builtin!
#endif
    return result;

#if 0
    f32 result = 1.0f;
    f32 x = value;
    result += ;
    value *= value;
    result += value * c0;
    //value *
    result += value
#endif
}

#if 0
internal f32
log(f32 value)
{

}

internal f32
log10(f32 value)
{

}
#endif

internal f32
pow(f32 x, f32 y)
{
    // TODO(michiel): pow(x, y) = e^(y * ln(x))
    f32 result;
#if NO_INTRINSICS
    result = (f32)powf(x, y);
#elif __has_builtin(__builtin_powf)
    result = (f32)__builtin_powf(x, y);
#else
#error No powf builtin!
#endif
    return result;
}

internal f32
square_root(f32 value)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)sqrtf(value);
#elif __has_builtin(__builtin_sqrtf)
    result = (f32)__builtin_sqrtf(value);
#else
#error No sqrtf builtin!
#endif
    return result;
}

internal f32
cos_f32_approx8_small(f32 angle)
{
    i_expect(-(0.5f + 0.000001f) <= angle);
    i_expect(angle <= (0.5f + 0.000001f));
    // cos(x) = 1 - (x^2/2!) + (x^4/4!) - (x^6/6!) + (x^8/8!)
    // cos(x) = 1 - x^2 (1/2! + x^2 (1/4! - x^2 (1/6! + x^2/8!)))

    f64 piSqOver4  = square(F64_PI) / 4.0;
    f64 pi4Over16  = square(piSqOver4);
    f64 pi6Over64  = pi4Over16 * piSqOver4;
    f64 pi8Over256 = square(pi4Over16);

    f32 c0 = -piSqOver4 / (2.0 * 1.0);
    f32 c1 = pi4Over16 / (4.0 * 3.0 * 2.0 * 1.0);
    f32 c2 = -pi6Over64 / (6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f32 c3 = pi8Over256 / (8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);

    f32 angSq = square(angle);
    f32 result = 1.0f + angSq * (c0 + angSq * (c1 + angSq * (c2 + c3 * angSq)));
    return result;
}

internal f32
sin_f32_approx7_small(f32 angle)
{
    i_expect(-(0.5f + 0.000001f) <= angle);
    i_expect(angle <= (0.5f + 0.000001f));
    // sin(x) = x - (x^3/3!) + (x^5/5!) - (x^7/7!) + (x^9/9!)
    // sin(x) = x (1 - x^2 (1/3! + x^2 (1/5! - x^2 (1/7! + x^2/9!)))

    f64 piSqOver4  = square(F64_PI) / 4.0;
    f64 pi4Over16  = square(piSqOver4);
    f64 pi6Over64  = pi4Over16 * piSqOver4;
    f64 pi8Over256 = square(pi4Over16);

    f32 c0 = -piSqOver4 / (3.0 * 2.0 * 1.0);
    f32 c1 = pi4Over16 / (5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f32 c2 = -pi6Over64 / (7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f32 c3 = pi8Over256 / (9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);

    f32 angSq = square(angle);
    f32 result = 0.5f * F32_PI * angle * (1.0f + angSq * (c0 + angSq * (c1 + angSq * (c2 + c3 * angSq))));
    return result;
}

internal f64
cos_f64_approx8_small(f64 angle)
{
    i_expect(-(0.5 + 0.00000001) <= angle);
    i_expect(angle <= (0.5 + 0.00000001));

    f64 piSqOver4  = square(F64_PI) / 4.0;
    f64 pi4Over16  = square(piSqOver4);
    f64 pi6Over64  = pi4Over16 * piSqOver4;
    f64 pi8Over256 = square(pi4Over16);

    f64 c0 = -piSqOver4 / (2.0 * 1.0);
    f64 c1 = pi4Over16 / (4.0 * 3.0 * 2.0 * 1.0);
    f64 c2 = -pi6Over64 / (6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f64 c3 = pi8Over256 / (8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);

    f64 angSq = square(angle);
    f64 result = 1.0 + angSq * (c0 + angSq * (c1 + angSq * (c2 + c3 * angSq)));
    return result;
}

internal f64
sin_f64_approx7_small(f64 angle)
{
    i_expect(-(0.5 + 0.00000001) <= angle);
    i_expect(angle <= (0.5 + 0.00000001));

    f64 piSqOver4  = square(F64_PI) / 4.0;
    f64 pi4Over16  = square(piSqOver4);
    f64 pi6Over64  = pi4Over16 * piSqOver4;
    f64 pi8Over256 = square(pi4Over16);

    f64 c0 = -piSqOver4 / (3.0 * 2.0 * 1.0);
    f64 c1 = pi4Over16 / (5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f64 c2 = -pi6Over64 / (7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);
    f64 c3 = pi8Over256 / (9.0 * 8.0 * 7.0 * 6.0 * 5.0 * 4.0 * 3.0 * 2.0 * 1.0);

    f64 angSq = square(angle);
    f64 result = 0.5 * F64_PI * angle * (1.0 + angSq * (c0 + angSq * (c1 + angSq * (c2 + c3 * angSq))));
    return result;
}

internal f32
cos_f32(f32 angle)
{
    f32 result = 0;
    angle += 0.125f;              // Map [0, 4] to [0.5, 4.5]
    angle = modulus01(angle);     // Mod to [0, 4]
    angle -= 0.125f;              // Map [0, 4] to [-0.5, 3.5]
    angle = absolute(angle * 4.0f);

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

    return result;
}

internal f64
sin_f64(f64 angle)
{
    f64 result = cos_f64(angle - 0.25);
    return result;
}

internal f32
cos(f32 radians)
{
    f32 oneOverTau = 1.0f / F32_TAU;
    f32 result = cos_f32(radians * oneOverTau);
    return result;
}

internal f64
cos(f64 radians)
{
    f64 oneOverTau = 1.0 / F64_TAU;
    f64 result = cos_f64(radians * oneOverTau);
    return result;
}

internal f32
sin(f32 radians)
{
    f32 oneOverTau = 1.0f / F32_TAU;
    f32 result = sin_f32(radians * oneOverTau);
    return result;
}

internal f64
sin(f64 radians)
{
    f64 oneOverTau = 1.0 / F64_TAU;
    f64 result = sin_f64(radians * oneOverTau);
    return result;
}

internal f32
tan(f32 angle)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)tanf(angle);
#elif __has_builtin(__builtin_tanf)
    result = (f32)__builtin_tanf(angle);
#else
#error No tan2f builtin!
#endif
    return result;
}

internal f32
asin(f32 angle)
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
acos(f32 angle)
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
atan(f32 angle)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)atanf(angle);
#elif __has_builtin(__builtin_atanf)
    result = (f32)__builtin_atanf(angle);
#else
#error No atan2f builtin!
#endif
    return result;
}

internal f32
atan2(f32 y, f32 x)
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
log(f32 x)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)logf(x);
#elif __has_builtin(__builtin_logf)
    result = (f32)__builtin_logf(x);
#else
#error No logf builtin!
#endif
    return result;
}

internal u32
rotate_left(u32 value, s32 amount)
{
#if COMPILER_MSVC
    u32 result = _rotl(value, amount);
#else
    amount &= 31;
    u32 result = (value << amount) | (value >> (32 - amount));
#endif
    return result;
}

internal u32
rotate_right(u32 value, s32 amount)
{
#if COMPILER_MSVC
    u32 result = _rotl(value, amount);
#else
    amount &= 31;
    u32 result = (value >> amount) | (value << (32 - amount));
#endif
    return result;
}

internal s32
s32_from_f32_round(f32 number)
{
    s32 result = (s32)round(number);
    return result;
}

internal u32
u32_from_f32_round(f32 number)
{
    u32 result = (u32)round(number);
    return result;
}

internal s32
s32_from_f32_floor(f32 number)
{
    s32 result = (s32)floor(number);
    return result;
}

internal u32
u32_from_f32_floor(f32 number)
{
    u32 result = (u32)floor(number);
    return result;
}

internal s32
s32_from_f32_ceil(f32 number)
{
    s32 result = (s32)ceil(number);
    return result;
}

internal u32
u32_from_f32_ceil(f32 number)
{
    u32 result = (u32)ceil(number);
    return result;
}

internal s32
s32_from_f32_truncate(f32 number)
{
    return (s32)number;
}

internal u32
u32_from_f32_truncate(f32 number)
{
    return (u32)number;
}

internal s64
s64_from_f64_round(f64 number)
{
    s64 result = (s64)round(number);
    return result;
}

internal u64
u64_from_f64_round(f64 number)
{
    u64 result = (u64)round(number);
    return result;
}

internal s64
s64_from_f64_floor(f64 number)
{
    s64 result = (s64)floor(number);
    return result;
}

internal u64
u64_from_f64_floor(f64 number)
{
    u64 result = (u64)floor(number);
    return result;
}

internal s64
s64_from_f64_ceil(f64 number)
{
    s64 result = (s64)ceil(number);
    return result;
}

internal u64
u64_from_f64_ceil(f64 number)
{
    u64 result = (u64)ceil(number);
    return result;
}

internal s64
s64_from_f64_truncate(f64 number)
{
    return (s64)number;
}

internal u64
u64_from_f64_truncate(f64 number)
{
    return (u64)number;
}

struct BitScanResult
{
    b32 found;
    u32 index;
};
internal BitScanResult
find_least_significant_set_bit(u32 value)
{
    BitScanResult result = {};

#if COMPILER_MSVC
    result.found = _BitScanForward((unsigned long *)&result.index, value);
#elif __has_builtin(__builtin_ctz)
    if (value)
    {
        result.index = __builtin_ctz(value);
        result.found = true;
    }
#else
    for(s32 test = 0; test < 32; ++test)
    {
        if(value & (1 << test))
        {
            result.index = test;
            result.found = true;
            break;
        }
    }
#endif

    return result;
}

internal BitScanResult
find_most_significant_set_bit(u32 value)
{
    BitScanResult result = {};

#if COMPILER_MSVC
    result.found = _BitScanReverse((unsigned long *)&result.index, value);
#elif __has_builtin(__builtin_clz)
    if (value)
    {
        result.index = 31 - __builtin_clz(value);
        result.found = true;
    }
#else
    for(s32 test = 31; test >= 0; --test)
    {
        if(value & (1 << test))
        {
            result.index = test;
            result.found = true;
            break;
        }
    }
#endif

    return result;
}

internal v4
sRGB_linearize(v4 c)
{
    v4 result;

    result.r = square(c.r);
    result.g = square(c.g);
    result.b = square(c.b);
    result.a = c.a;

    return result;
}

internal v4
sRGB_linearize(f32 r, f32 g, f32 b, f32 a)
{
    v4 input = {r, g, b, a};
    v4 result = sRGB_linearize(input);
    return result;
}

internal v4
sRGB_from_linear(v4 c)
{
    v4 result;

    result.r = square_root(c.r);
    result.g = square_root(c.g);
    result.b = square_root(c.b);
    result.a = c.a;

    return result;
}

internal v4
linear1_from_sRGB255(v4 c)
{
    v4 result;

    f32 inv255 = 1.0f / 255.0f;

    result.r = square(inv255*c.r);
    result.g = square(inv255*c.g);
    result.b = square(inv255*c.b);
    result.a = inv255*c.a;

    return result;
}

internal v4
sRGB255_from_linear1(v4 c)
{
    v4 result;

    f32 one255 = 255.0f;

    result.r = one255*square_root(c.r);
    result.g = one255*square_root(c.g);
    result.b = one255*square_root(c.b);
    result.a = one255*c.a;

    return result;
}
