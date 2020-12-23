#if NO_INTRINSICS
// TODO(michiel): Remove dependency
#include <math.h>
#endif

// NOTE(michiel): Use this define to overwrite sin/cos/tan functions
#ifndef TRIGONOMETRY_FILE
#define TRIGONOMETRY_FILE "trigonometry_v2.h"
#endif

#ifndef MATH_USE_SSE4_FUNC
#define MATH_USE_SSE4_FUNC 1
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
    while (((u32)1 << bitPos) <= value)
    {
        ++bitPos;
    }
    return bitPos;
}

internal u32
log2(u32 value)
{
    u32 result = 0;
    while (value > 1)
    {
        value /= 2;
        ++result;
    }
    return result;
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

internal f64
lerp(f64 min, f64 t, f64 max)
{
    f64 result = min;
    result += (max - min) * t;
    return result;
}

internal f32
map(f32 value, f32 fromMin, f32 fromMax, f32 toMin, f32 toMax)
{
    f32 result;
    result = (value - fromMin) / (fromMax - fromMin);
    result = result * (toMax - toMin) + toMin;
    return result;
}

internal f64
map(f64 value, f64 fromMin, f64 fromMax, f64 toMin, f64 toMax)
{
    f64 result;
    result = (value - fromMin) / (fromMax - fromMin);
    result = result * (toMax - toMin) + toMin;
    return result;
}

internal v2
map(v2 value, v2 fromMin, v2 fromMax, v2 toMin, v2 toMax)
{
    v2 result;
    result.x = map(value.x, fromMin.x, fromMax.x, toMin.x, toMax.x);
    result.y = map(value.y, fromMin.y, fromMax.y, toMin.y, toMax.y);
    return result;
}

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
    U32F32 result;
    result.f = value;
    result.u &= ~F32_SIGN_MASK;
    return result.f;
}

internal f64
absolute(f64 value)
{
    U64F64 result;
    result.f = value;
    result.u &= ~F64_SIGN_MASK;
    return result.f;
}

internal s32
sign_of(s32 value)
{
    s32 result = (value >> 31) | 0x1;
    return result;
}

internal s64
sign_of(s64 value)
{
    s32 result = (value >> 63) | 0x1;
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
truncate32(f32 value)
{
    f32 result;
    result = (f32)(s32)value;
    return result;
}

internal f64
truncate64(f64 value)
{
    f64 result;
    result = (f64)(s64)value;
    return result;
}

#if MATH_USE_SSE4_FUNC
union FloatMathVec
{
    __m128  mf;
    __m128d md;
    __m128i mi;
    f32 ef32[4];
    f64 ef64[2];
    s32 ei32[4];
    u32 eu32[4];
};
#endif

internal f32
floor32(f32 value)
{
#if MATH_USE_SSE4_FUNC
    FloatMathVec result;
    result.mf = _mm_set1_ps(value);
    result.mf = _mm_floor_ps(result.mf);
    return result.ef32[0];
#else
    // TODO(michiel): Precision check
    f32 result;
    result = truncate(value + (value < 0.0f ? -0.9999999f : 0.0));
    return result;
#endif
}

internal f64
floor64(f64 value)
{
#if MATH_USE_SSE4_FUNC
    FloatMathVec result;
    result.md = _mm_set1_pd(value);
    result.md = _mm_floor_pd(result.md);
    return result.ef64[0];
#else
    f64 result;
    result = truncate(value + (value < 0.0 ? -0.999999999999999 : 0.0));
    return result;
#endif
}

internal f32
ceil32(f32 value)
{
#if MATH_USE_SSE4_FUNC
    FloatMathVec result;
    result.mf = _mm_set1_ps(value);
    result.mf = _mm_ceil_ps(result.mf);
    return result.ef32[0];
#else
    f32 result;
    result = truncate(value + (value < 0.0f ? 0.0f : 0.9999999f));
    return result;
#endif
}

internal f64
ceil64(f64 value)
{
#if MATH_USE_SSE4_FUNC
    FloatMathVec result;
    result.md = _mm_set1_pd(value);
    result.md = _mm_ceil_pd(result.md);
    return result.ef64[0];
#else
    f64 result;
    result = truncate(value + (value < 0.0 ? 0.0 : 0.999999999999999));
    return result;
#endif
}

internal f32
round32(f32 value)
{
#if MATH_USE_SSE4_FUNC
    FloatMathVec result;
    result.mf = _mm_set1_ps(value);
    result.mf = _mm_round_ps(result.mf, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    return result.ef32[0];
#else
    f32 result;
    result = truncate(value + 0.5f * sign_of(value));
    return result;
#endif
}

internal f64
round64(f64 value)
{
#if MATH_USE_SSE4_FUNC
    FloatMathVec result;
    result.md = _mm_set1_pd(value);
    result.md = _mm_round_pd(result.md, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    return result.ef64[0];
#else
    f64 result;
    result = truncate(value + 0.5 * sign_of(value));
    return result;
#endif
}

internal f32
fraction32(f32 value)
{
    f32 result;
    result = value - floor32(value);
    return result;
}

internal f64
fraction64(f64 value)
{
    f64 result;
    result = value - floor64(value);
    return result;
}

#if !COMPILER_MSVC
// TODO(michiel): Add windows variant
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
#endif

internal f32
modulus01(f32 f)
{
    f32 result;
    result = f - floor32(f);
    return result;
}

internal f64
modulus01(f64 f)
{
    f64 result;
    result = f - floor64(f);
    return result;
}

internal f32
modulus(f32 x, f32 y)
{
    i_expect(y != 0.0f);
    f32 result;
    result = x - floor32(x / y) * y;
    return result;
}

internal f64
modulus(f64 x, f64 y)
{
    i_expect(y != 0.0);
    f64 result;
    result = x - floor64(x / y) * y;
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

#if __has_builtin(__builtin_exp)
internal f64
exp(f64 f)
{
    f64 result;
#if __has_builtin(__builtin_exp)
    result = __builtin_exp(f);
#else
#error No exp builtin!
#endif
    return result;
}
#endif

internal f32
pow(f32 x, f32 y)
{
    // TODO(michiel): pow(x, y) = e^(y * ln(x))
    f32 result;
#if NO_INTRINSICS
    result = powf(x, y);
#elif __has_builtin(__builtin_powf)
    result = __builtin_powf(x, y);
#else
#error No powf builtin!
#endif
    return result;
}

#if !NO_INTRINSICS
internal f64
pow(f64 x, f64 y)
{
    // TODO(michiel): pow(x, y) = e^(y * ln(x))
    f64 result;
#if __has_builtin(__builtin_pow)
    result = __builtin_pow(x, y);
#else
#error No pow builtin!
#endif
    return result;
}
#endif

internal f32
square_root(f32 value)
{
    f32 result;
#if NO_INTRINSICS
    result = sqrtf(value);
#elif __has_builtin(__builtin_sqrtf)
    result = __builtin_sqrtf(value);
#else
#error No sqrtf builtin!
#endif
    return result;
}

internal f64
square_root(f64 value)
{
    f64 result;
#if NO_INTRINSICS
    result = sqrt(value);
#elif __has_builtin(__builtin_sqrt)
    result = __builtin_sqrt(value);
#else
#error No sqrt builtin!
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

#if !NO_INTRINSICS
internal f64
log(f64 x)
{
    f64 result;
#if __has_builtin(__builtin_log)
    result = __builtin_log(x);
#else
#error No log builtin!
#endif
    return result;
}
#endif

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

internal u64
rotate_left(u64 value, s32 amount)
{
#if COMPILER_MSVC
    u64 result = _rotl64(value, amount);
#else
    amount &= 63;
    u64 result = (value << amount) | (value >> (64 - amount));
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

#include TRIGONOMETRY_FILE

internal s16
s16_from_f32_round(f32 number)
{
    s16 result = (s16)round32(number);
    return result;
}

internal u16
u16_from_f32_round(f32 number)
{
    u16 result = (u16)round32(number);
    return result;
}

internal s16
s16_from_f32_floor(f32 number)
{
    s16 result = (s16)floor32(number);
    return result;
}

internal u16
u16_from_f32_floor(f32 number)
{
    u16 result = (u16)floor32(number);
    return result;
}

internal s16
s16_from_f32_ceil(f32 number)
{
    s16 result = (s16)ceil32(number);
    return result;
}

internal u16
u16_from_f32_ceil(f32 number)
{
    u16 result = (u16)ceil32(number);
    return result;
}

internal s16
s16_from_f32_truncate(f32 number)
{
    return (s16)number;
}

internal u16
u16_from_f32_truncate(f32 number)
{
    return (u16)number;
}

internal s32
s32_from_f32_round(f32 number)
{
    s32 result = (s32)round32(number);
    return result;
}

internal u32
u32_from_f32_round(f32 number)
{
    u32 result = (u32)round32(number);
    return result;
}

internal s32
s32_from_f32_floor(f32 number)
{
    s32 result = (s32)floor32(number);
    return result;
}

internal u32
u32_from_f32_floor(f32 number)
{
    u32 result = (u32)floor32(number);
    return result;
}

internal s32
s32_from_f32_ceil(f32 number)
{
    s32 result = (s32)ceil32(number);
    return result;
}

internal u32
u32_from_f32_ceil(f32 number)
{
    u32 result = (u32)ceil32(number);
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
    s64 result = (s64)round64(number);
    return result;
}

internal u64
u64_from_f64_round(f64 number)
{
    u64 result = (u64)round64(number);
    return result;
}

internal s64
s64_from_f64_floor(f64 number)
{
    s64 result = (s64)floor64(number);
    return result;
}

internal u64
u64_from_f64_floor(f64 number)
{
    u64 result = (u64)floor64(number);
    return result;
}

internal s64
s64_from_f64_ceil(f64 number)
{
    s64 result = (s64)ceil64(number);
    return result;
}

internal u64
u64_from_f64_ceil(f64 number)
{
    u64 result = (u64)ceil64(number);
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

internal f32
safe_ratio_n(f32 num, f32 den, f32 N)
{
    f32 result = N;
    if (den != 0.0f)
    {
        result = num / den;
    }
    return result;
}

internal f32
safe_ratio0(f32 num, f32 den)
{
    return safe_ratio_n(num, den, 0.0f);
}

internal f32
safe_ratio1(f32 num, f32 den)
{
    return safe_ratio_n(num, den, 1.0f);
}
