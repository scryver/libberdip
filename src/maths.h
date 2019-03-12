#if (!__has_builtin(__builtin_sinf) || !__has_builtin(__builtin_cosf) || !__has_builtin(__builtin_sqrtf))

// TODO(michiel): Remove dependency
#include <math.h>
#define sin  sinf
#define cos  cosf
#define sqrt sqrtf
//#define exp  expf
#define exp  fast_expf
#define log  logf

#else

#define sin  __builtin_sinf
#define cos  __builtin_cosf
#define sqrt __builtin_sqrtf
//#define exp  __builtin_expf
#define exp  fast_expf
#define log     __builtin_logf
#define floor(f)                __builtin_floorf(f)
#define round(f)                ((s32)((f) + ((f) < 0.0f ? -0.5f : 0.5f)))
//#define round                   __builtin_roundf
#define trunc(f)                ((s32)(f))
#define fabs(f)                 __builtin_fabsf(f)

#define powd  __builtin_pow

#endif

internal inline f32
fast_expf_(f32 x, u32 approx)
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
fast_expf(f32 x)
{
    return fast_expf_(x, 12);
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
