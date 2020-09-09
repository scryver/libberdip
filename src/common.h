// TODO(michiel): @Remove <stdio> printf and FILE *
#include <stdio.h>

#include <stdarg.h>    // va_start, va_end
#include <stddef.h>    // size_t, ssize_t
#include <stdint.h>    // uint*_t, int*_t
#include <float.h>     // FLT_MIN, FLT_MAX, DBL_MIN, DBL_MAX

#if !defined(LIBBERDIP_EXPECT)
#define LIBBERDIP_EXPECT 1
#endif

#if !defined(NO_INTRINSICS)
#define NO_INTRINSICS 0
#endif

#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !defined(COMPILER_GCC)
#define COMPILER_GCC 0
#endif

#if !defined(COMPILER_TCC)
#define COMPILER_TCC 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM && !COMPILER_GCC && !COMPILER_TCC
#if _MSC_VER
#undef  COMPILER_MSVC
#define COMPILER_MSVC 1
#elif __clang__
#undef  COMPILER_LLVM
#define COMPILER_LLVM 1
#elif __GNUC__
#undef  COMPILER_GCC
#define COMPILER_GCC 1
#elif __TINYC__
#undef  COMPILER_TCC
#define COMPILER_TCC 1
// TODO(michiel): - Add default argument values
//                - Add function overload
//                - Make structs typedeffed by default
#else
#error Unknown compiler
#endif
#endif

#ifndef __has_builtin                   // Optional of course.
#define __has_builtin(x)                0  // Compatibility with non-clang compilers.
#endif

#define internal static
#define global   static
#define persist  static
#define unused(x) (void)x

#ifndef __cplusplus
#define false    0
#define true     1
#endif

// TODO(michiel): @Remove printf
#if LIBBERDIP_EXPECT
#if __has_builtin(__builtin_trap)
#define i_expect_simple(expr)   ((expr) ? (void)0 : __builtin_trap())
#define i_expect(expr)          if (!(expr)) { fprintf(stderr, "%s:%d:Expectation failed: '%s'\n", __FILE__, __LINE__, #expr); __builtin_trap(); }
#else
#define i_expect_simple(expr)   ((expr) ? (void)0 : (*(volatile int *)0 = 0))
#define i_expect(expr)          if (!(expr)) { fprintf(stderr, "%s:%d:Expectation failed: '%s'\n", __FILE__, __LINE__, #expr); *(volatile int *)0 = 0; }
#endif
#else  // LIBBERDIP_EXPECT
#define i_expect_simple(expr)
#define i_expect(expr)
#endif // LIBBERDIP_EXPECT

#ifdef __cplusplus
#define compile_expect(expr)    static_assert(expr, "Expectation failed: " #expr)
#else
#define compile_expect(expr) _Static_assert(expr, "Expectation failed: " #expr)
#endif

#if LIBBERDIP_EXPECT
#define NOT_IMPLEMENTED      i_expect(0 && "Not implemented!")
#else
#define NOT_IMPLEMENTED      compile_expect(0 && "Not implemented!")
#endif

#define INVALID_CODE_PATH    i_expect(0 && "Invalid code path")
#define INVALID_DEFAULT_CASE default: { i_expect(0 && "Invalid default case"); } break

#include "types.h"

#define U8_MAX   0xFF
#define U16_MAX  0xFFFF
#define U32_MAX  0xFFFFFFFF
#define U64_MAX  0xFFFFFFFFFFFFFFFFULL

#define S8_MIN   ((s8)0x80)
#define S8_MAX   ((s8)0x7F)
#define S16_MIN  ((s16)0x8000)
#define S16_MAX  ((s16)0x7FFF)
#define S32_MAX  ((s32)0x7FFFFFFF)
#define S32_MIN  ((s32)0x80000000)
#define S64_MAX  ((s64)0x7FFFFFFFFFFFFFFFLL)
#define S64_MIN  ((s64)0x8000000000000000LL)

// TODO(michiel): Find a better solution to the casting...
typedef union u32f32 { u32 u; f32 f; } U32F32;
internal U32F32 u32f32(u32 u) { U32F32 t; t.u = u; return t; }
#define F32_SIGN_MASK           0x80000000U
#define F32_EXP_MASK            0x7F800000U
#define F32_FRAC_MASK           0x007FFFFFU
#if 1
#define F32_MAX                 u32f32(0x7F000000U).f
#define F32_INF                 u32f32(F32_EXP_MASK).f
#define F32_NAN                 u32f32(F32_EXP_MASK | F32_FRAC_MASK).f // NOTE(michiel): F32_FRAC_MASK can be anything but 0
#else
#define F32_MAX                 FLT_MAX   // NOTE(michiel): Sign bit 0, Exponent (8bit) 254,     Mantissa all 1's
#define F32_INF                 3.403e38  // NOTE(michiel): Sign bit 0, Exponent (8bit) all 1's, Mantissa all 0's
#define F32_NAN
#endif

typedef union u64f64 { u64 u; f64 f; } U64F64;
internal U64F64 u64f64(u64 u) { U64F64 t; t.u = u; return t; }
#define F64_SIGN_MASK           0x8000000000000000ULL
#define F64_EXP_MASK            0x7FF0000000000000ULL
#define F64_FRAC_MASK           0x000FFFFFFFFFFFFFULL
#if 1
#define F64_MAX                 u64f64(0x7F00000000000000ULL).f
#define F64_INF                 u64f64(F64_EXP_MASK).f
#define F64_NAN                 u64f64(F64_EXP_MASK | F64_FRAC_MASK).f // NOTE(michiel): F64_FRAC_MASK can be anything but 0
#else
#define F64_MAX                 DBL_MAX
// TODO(michiel): Inf
#endif

#define F80_PI   3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748L
#define F80_E    2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274274663919320030599218174135966290435729003342952605956307381323286279434907632338298807531952510190115738341879307021540891499348841675092447614606680822648001684774118537423454424371075390777449920695517027618386062613314L
#define F64_PI                  ((f64)F80_PI)
#define F32_PI                  ((f32)F80_PI)
#define F64_TAU                 (2.0 * (f64)F80_PI)
#define F32_TAU                 (2.0f * (f32)F80_PI)
#define F64_E                   ((f64)F80_E)
#define F32_E                   ((f32)F80_E)

//
// NOTE(michiel): Constants _borrowed_ from cephes library
#define F32_FOUR_OVER_PI     1.27323954473516f
#define F32_PI_OVER_2        1.5707963267948966192f
#define F32_PI_OVER_4        0.7853981633974483096f
//

#define offset_of(type, member) (umm)&(((type *)0)->member)

#define array_count(x)          (sizeof(x) / sizeof(*x))

#define is_pow2(x)              (((x) != 0) && (((x) & ((x)-1)) == 0))
//#define is_pow2(x)              ((x) && (((x) & ~((x)-1)) == (x)))

#define align_down(x, a)        ((x) & ~((a) - 1))
#define align_up(x, a)          align_down((x) + (a) - 1, (a))
#define align4(x)               (((x) +  3) & ~3U)
#define align8(x)               (((x) +  7) & ~7U)
#define align16(x)              (((x) + 15) & ~15U)
#define align_ptr_down(p, a)    ((void *)align_down((umm)(p), (a)))
#define align_ptr_up(p, a)      ((void *)align_up((umm)(p), (a)))

#define u64_from_ptr(p)         ((u64)(umm)(p))
#define ptr_from_u64(u)         ((void *)(umm)(u))

#define rad2deg(angle)          (((angle) / F80_PI) * 180)
#define deg2rad(angle)          (((angle) / 180) * F80_PI)

#define minimum(a, b)           ((a) < (b) ? (a) : (b))
#define maximum(a, b)           ((a) > (b) ? (a) : (b))
#define clamp(min, x, max)      (maximum((min), minimum((max), (x))))

#define minimum3(a, b, c)       minimum((a), minimum((b), (c)))
#define maximum3(a, b, c)       maximum((a), maximum((b), (c)))
#define minimum4(a, b, c, d)    minimum(minimum((a), (b)), minimum((c), (d)))
#define maximum4(a, b, c, d)    maximum(maximum((a), (b)), maximum((c), (d)))

#define kilobytes(kB)           ((kB) * 1024LL)
#define megabytes(MB)           (kilobytes(MB) * 1024LL)
#define gigabytes(GB)           (megabytes(GB) * 1024LL)
#define terabytes(TB)           (gigabytes(TB) * 1024LL)

internal u32 safe_truncate_to_u32(u64 value) { i_expect(value <= U32_MAX); return (u32)(value & U32_MAX); }
internal u16 safe_truncate_to_u16(u64 value) { i_expect(value <= U16_MAX); return (u16)(value & U16_MAX); }
internal u8  safe_truncate_to_u8(u64 value)  { i_expect(value <= U8_MAX);  return (u8)(value & U8_MAX); }
internal s32 safe_truncate_to_s32(s64 value) { i_expect(value <= (s64)S32_MAX); i_expect(value >= (s64)S32_MIN); return (s32)value; }
internal s16 safe_truncate_to_s16(s64 value) { i_expect(value <= (s64)(s32)S16_MAX); i_expect(value >= (s64)(s32)S16_MIN); return (s16)value; }
internal s8  safe_truncate_to_s8(s64 value)  { i_expect(value <= (s64)(s32)(s16)S8_MAX); i_expect(value >= (s64)(s32)(s16)S8_MIN); return (s8)value; }

internal u32
reverse_bits(u32 b, u32 msb)
{
    u32 mask = (1 << msb) - 1;
    u32 result = b;
    --msb;
    for (b >>= 1; b; b>>= 1)
    {
        result <<= 1;
        result |= b & 1;
        --msb;
    }
    result <<= msb;
    return result & mask;
}

typedef struct BitScanResult
{
    b32 found;
    u32 index;
} BitScanResult;

internal BitScanResult
find_least_significant_set_bit(u32 value)
{
    BitScanResult result = {0};

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
    BitScanResult result = {0};

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

internal BitScanResult
find_least_significant_set_bit(u64 value)
{
    BitScanResult result = {0};

    for(s32 test = 0; test < 64; ++test)
    {
        if(value & (1LL << test))
        {
            result.index = test;
            result.found = true;
            break;
        }
    }

    return result;
}

internal BitScanResult
find_most_significant_set_bit(u64 value)
{
    BitScanResult result = {0};

    for(s32 test = 63; test >= 0; --test)
    {
        if(value & (1LL << test))
        {
            result.index = test;
            result.found = true;
            break;
        }
    }

    return result;
}

//#define copy(n, s, d)  copy_(n*sizeof(s[0]), s, d)
#define copy_struct(s, d)    copy(sizeof(*(s)), s, d)
#define copy_array(c, s, d)  copy(sizeof(*(s))*c, s, d)

internal umm
copy(umm size, const void *src, void *dst)
{
#if 0
    if (size > 3)
    {
        if(((u8 *)dst < ((u8 *)src - 4)) ||
           ((u8 *)dst >= ((u8 *)src + size)))
        {
            u32 *src32 = (u32 *)src;
            u32 *dst32 = (u32 *)dst;
            umm size4 = size >> 2; // size/4
            umm rem = size - (size4 << 2);
            while(size4--) {
                *dst32++ = *src32++;
            }
            u8 *s = (u8 *)src32;
            u8 *d = (u8 *)dst32;
            while (rem--) {
                *d++ = *s++;
            }
        }
        else
        {
            umm size4 = size >> 2;
            umm rem = size - (size4 << 2);

            u8 *s = (u8 *)src + size;
            u8 *d = (u8 *)dst + size;
            while (rem--) {
                *(--d) = *(--s);
            }

            u32 *src32 = (u32 *)s;
            u32 *dst32 = (u32 *)d;

            while (size4--) {
                *(--dst32) = *(--src32);
            }
        }
    }
    else if (dst < src)
    {
        u8 *s = (u8 *)src;
        u8 *d = (u8 *)dst;
        while (size--) {
            *d++ = *s++;
        }
    }
    else
    {
        u8 *s = (u8 *)src + size;
        u8 *d = (u8 *)dst + size;
        while (size--) {
            *(--d) = *(--s);
        }
    }
#else
    u8 *s = (u8 *)src;
    u8 *d = (u8 *)dst;
    if (d < s)
    {
        while (size--)
        {
            *d++ = *s++;
        }
    }
    else if (d > s)
    {
        s += size;
        d += size;
        while (size--)
        {
            *(--d) = *(--s);
        }
    }
#endif

    return size;
}

internal void
copy_single(umm size, u32 value, void *dst)
{
    u8 value8 = value & 0xFF;
    u32 value32 = (value8 << 24) | (value << 16) | (value << 8) | value;
    u32 *dst32 = (u32 *)dst;
    umm size4 = size >> 2; // size/4
    umm rem = size - (size4 << 2);
    while(size4--) {
        *dst32++ = value32;
    }
    u8 *d = (u8 *)dst32;
    while (rem--) {
        *d++ = value8;
    }
}

internal f32
clamp01(f32 value)
{
    f32 result = clamp(0.0f, value, 1.0f);
    return result;
}

#ifdef __cplusplus
internal f64
clamp01(f64 value)
{
    f64 result = clamp(0.0, value, 1.0);
    return result;
}
#endif

#ifdef __cplusplus
internal Buffer
advance(Buffer b, u32 amount = 1)
{
    Buffer result = b;
    result.size -= amount;
    result.data += amount;
    return result;
}

internal void
advance(Buffer *b, u32 amount = 1)
{
    b->size -= amount;
    b->data += amount;
}

internal Buffer
save_advance(Buffer b, u32 amount)
{
    if (amount > b.size) {
        amount = b.size;
    }
    return advance(b, amount);
}

#else

internal Buffer
advance_sub(Buffer b, u32 amount)
{
    Buffer result = b;
    result.size -= amount;
    result.data += amount;
    return result;
}

internal void
advance(Buffer *b, u32 amount)
{
    b->size -= amount;
    b->data += amount;
}

internal Buffer
save_advance(Buffer b, u32 amount)
{
    if (amount > b.size) {
        amount = b.size;
    }
    return advance_sub(b, amount);
}

#endif

//
// NOTE(michiel): Hashing
//

// TODO(michiel): Better hash functions
internal inline u64
hash_u64(u64 x) {
    x *= 0xFF51AFD7ED558CCD;
    x ^= x >> 32;
    return x;
}
internal inline u64 hash_ptr(void *ptr)    { return hash_u64((umm)ptr); }
internal inline u64 hash_mix(u64 x, u64 y) { return hash_u64(x ^ y); }

internal inline u64
hash_bytes(void *ptr, umm len) {
    u64 x = 0xCBF29CE484222325;
    char *buf = (char *)ptr;
    for (umm i = 0; i < len; i++) {
        x ^= buf[i];
        x *= 0x100000001B3;
        x ^= x >> 32;
    }
    return x;
}

#include "vector_types.h"

#ifdef __cplusplus
#include "./common_cpp.h" // Overloaded functions and such things c won't handle
#endif

