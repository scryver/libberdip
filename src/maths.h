#if (!__has_builtin(__builtin_sinf) || !__has_builtin(__builtin_cosf) || !__has_builtin(__builtin_sqrtf))

// TODO(michiel): Remove dependency
#include <math.h>
#define sin  sinf
#define cos  cosf
//#define exp  expf
#define exp  fast_expf
#define log  logf

#else

#define sin  __builtin_sinf
#define cos  __builtin_cosf
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

internal f32
square(f32 f)
{
    f32 result = f * f;
    return result;
}

internal f32
square_root(f32 f)
{
    f32 result = 0.0f;
#if (!__has_builtin(__builtin_sqrtf))
    result = sqrtf(f);
#else
    result = __builtin_sqrtf(f);
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
