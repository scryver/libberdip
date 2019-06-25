#if NO_INTRINSICS
// TODO(michiel): Remove dependency
#include <math.h>
#endif

internal inline f32
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

internal inline f32
fast_exp(f32 x)
{
    return fast_exp_(x, 12);
}

internal inline u32
log2_up(u32 value)
{
    u32 bitPos = 0;
    while (((u64)1 << bitPos) <= value)
    {
        ++bitPos;
    }
    return bitPos;
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

internal s32
floor(s32 value)
{
    s32 result;
#if NO_INTRINSICS
    result = (s32)floor(value);
#elif __has_builtin(__builtin_floorl)
    result = (s32)__builtin_floorl(value);
#else
#error No floorl builtin!
#endif
    return result;
}

internal f32
floor(f32 value)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)floorf(value);
#elif __has_builtin(__builtin_floorf)
    result = (f32)__builtin_floorf(value);
#else
#error No floorf builtin!
#endif
    return result;
}

internal s32
ceil(s32 value)
{
    s32 result;
#if NO_INTRINSICS
    result = (s32)ceill(value);
#elif __has_builtin(__builtin_ceill)
    result = (s32)__builtin_ceill(value);
#else
#error No ceill builtin!
#endif
    return result;
}

internal f32
ceil(f32 value)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)ceilf(value);
#elif __has_builtin(__builtin_ceilf)
    result = (f32)__builtin_ceilf(value);
#else
#error No ceilf builtin!
#endif
    return result;
}

internal f32
round(f32 value)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)roundf(value);
#elif __has_builtin(__builtin_roundf)
    result = (f32)__builtin_roundf(value);
#else
#error No roundf builtin!
#endif
    return result;
}

internal s32
absolute(s32 value)
{
    s32 result = (value < 0) ? -value : value;
    return result;
}

internal f32
absolute(f32 value)
{
    f32 result = (value < 0.0f) ? -value : value;
    return result;
}

internal f32
modulus(f32 x, f32 y)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)fmod(x, y);
#elif __has_builtin(__builtin_fmod)
    result = (f32)__builtin_fmod(x, y);
#else
#error No fmod builtin!
#endif
    return result;
}

internal s32
square_root(s32 value)
{
    s32 result;
#if NO_INTRINSICS
    result = (s32)sqrtl(value);
#elif __has_builtin(__builtin_sqrtl)
    result = (s32)__builtin_sqrtl(value);
#else
#error No sqrtl builtin!
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

internal s32
pow(s32 x, s32 y)
{
    s32 result;
#if NO_INTRINSICS
    result = (s32)powl(x, y);
#elif __has_builtin(__builtin_powl)
    result = (s32)__builtin_powl(x, y);
#else
#error No powl builtin!
#endif
    return result;
}

internal f32
pow(f32 x, f32 y)
{
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
sin(f32 angle)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)sinf(angle);
#elif __has_builtin(__builtin_sinf)
    result = (f32)__builtin_sinf(angle);
#else
    // TODO(michiel): implement sinf
#error No sinf builtin!
#endif
    return result;
}

internal f32
cos(f32 angle)
{
    f32 result;
#if NO_INTRINSICS
    result = (f32)cosf(angle);
#elif __has_builtin(__builtin_cosf)
    result = (f32)__builtin_cosf(angle);
#else
#error No cosf builtin!
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

internal s32
sign_of(s32 value)
{
    s32 result = (value >= 0) ? 1 : -1;
    return result;
}

internal f32
sign_of(f32 value)
{
    f32 result = (value >= 0.0f) ? 1.0f : -1.0f;
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
