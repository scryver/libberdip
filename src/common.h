// TODO(michiel): @Remove <stdio> printf and FILE *
#include <stdio.h>
// TODO(michiel): @Remove <stdlib> calloc/realloc/malloc/free stuff
#include <stdlib.h>

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

#if !COMPILER_MSVC && !COMPILER_LLVM && !COMPILER_GCC
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#elif __clang__
#undef COMPILER_LLVM
#define COMPILER_LLVM 1
#elif __GNUC__
#undef COMPILER_GCC
#define COMPILER_GCC 1
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

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef int8_t   b8;
typedef int16_t  b16;
typedef int32_t  b32;
typedef int64_t  b64;

typedef float    f32;
typedef double   f64;

typedef unsigned long int umm;
typedef signed long int   smm;
compile_expect(sizeof(umm) == sizeof(size_t));

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

#define F32_SIGN_MASK           0x80000000
#define F32_EXP_MASK            0x7F800000
#define F32_FRAC_MASK           0x007FFFFF
#define F32_MAX                 FLT_MAX   // NOTE(michiel): Sign bit 0, Exponent (8bit) 254,     Mantissa all 1's
#define F32_MIN                -FLT_MAX   // NOTE(michiel): Sign bit 0, Exponent (8bit) 254,     Mantissa all 1's
#define F32_INF                 3.403e38 // NOTE(michiel): Sign bit 0, Exponent (8bit) all 1's, Mantissa all 0's
#define F32_MINF               -3.403e38 // NOTE(michiel): Sign bit 0, Exponent (8bit) all 1's, Mantissa all 0's

#define F64_SIGN_MASK           0x8000000000000000
#define F64_EXP_MASK            0x7FF0000000000000
#define F64_FRAC_MASK           0x000FFFFFFFFFFFFF
#define F64_MIN                -DBL_MAX
#define F64_MAX                 DBL_MAX
// TODO(michiel): Inf and -inf

#define LONG_PI   3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748
#define LONG_E    2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274274663919320030599218174135966290435729003342952605956307381323286279434907632338298807531952510190115738341879307021540891499348841675092447614606680822648001684774118537423454424371075390777449920695517027618386062613314
#define F64_PI                  ((f64)LONG_PI)
#define F32_PI                  ((f32)F64_PI)
#define F64_TAU                 (2.0f * (f64)LONG_PI)
#define F32_TAU                 ((f32)F64_TAU)
#define F64_E                   ((f64)LONG_E)
#define F32_E                   ((f32)F64_E)

#define enum8(name)             u8
#define enum16(name)            u16
#define enum32(name)            u32

#define offset_of(type, member) (umm)&(((type *)0)->member)

#define array_count(x)          (sizeof(x) / sizeof(*x))

#define is_pow2(x)              (((x) != 0) && (((x) & ((x)-1)) == 0))

#define align_down(x, a)        ((x) & ~((a) - 1))
#define align_up(x, a)          align_down((x) + (a) - 1, (a))
#define align4(x)               (((x) +  3) & ~3U)
#define align8(x)               (((x) +  7) & ~7U)
#define align16(x)              (((x) + 15) & ~15U)
#define align_ptr_down(p, a)    ((void *)align_down((umm)(p), (a)))
#define align_ptr_up(p, a)      ((void *)align_up((umm)(p), (a)))

#define u64_from_ptr(p)         ((u64)(umm)(p))
#define ptr_from_u64(u)         ((void *)(umm)(u))

#define rad2deg(angle)          (((angle) / LONG_PI) * 180)
#define deg2rad(angle)          (((angle) / 180) * LONG_PI)

#define minimum(a, b)           ((a) < (b) ? (a) : (b))
#define maximum(a, b)           ((a) > (b) ? (a) : (b))
#define clamp(min, x, max)      (maximum(min, minimum(max, x)))

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

internal umm
copy(umm size, const void *src, void *dst)
{
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

internal inline f32
clamp01(f32 value)
{
    f32 result = clamp(0.0f, value, 1.0f);
    return result;
}

// NOTE(michiel): Generic buffer (memory data and a size)
typedef struct Buffer
{
    umm size;
    u8 *data;
} Buffer;

// NOTE(michiel): Stretchy buffer header (prepended to keep track of num items etc)
#define BUF_MAGIC 0xB0FFE20F20F78D1E
typedef struct BufHdr
{
    u64 magic;
    u32 cap;
    u32 len;
    u8 data[1];
} BufHdr;

//
// NOTE(michiel): Memory allocation pool/arena, auto block growth
//

// NOTE(michiel): This block represents a single allocation, this data won't change after creation
typedef struct ArenaBlock
{
    struct ArenaBlock *next;
    umm size;
    u8 *mem;
} ArenaBlock;

// TODO(michiel): Random freeing of allocated data
// NOTE(michiel): This is the arena, which has a current and end pointer of the currently used
// ArenaBlock.
typedef struct Arena
{
    u8 *at;
    u8 *end;
    ArenaBlock sentinel;
} Arena;

// NOTE(michiel): Temporary memory, will deallocate every allocation when destroy_temporary() is called
typedef struct TempMemory
{
    Arena *arena;
    ArenaBlock *block;
    u8 *origAt;
    u8 *origEnd;
} TempMemory;

// NOTE(michiel): Image buffer (32bit pixels assumed)
typedef struct Image
{
    u32 width;
    u32 height;
    u32 *pixels;
} Image;

// NOTE(michiel): Image buffer (8bit pixels, grey/alpha), as used in some font instances
typedef struct Image8
{
    u32 width;
    u32 height;
    u8 *pixels;
} Image8;



//
// NOTE(michiel): Allocation
//

typedef enum AllocateFlags
{
    Alloc_NoClear = 0x01,
} AllocateFlags;

#define allocate_array(type, count, ...) (type *)allocate_size(sizeof(type)*count, ## __VA_ARGS__)
#define allocate_struct(type, ...)       (type *)allocate_size(sizeof(type), ## __VA_ARGS__)
internal inline void *allocate_size(umm size, u32 flags)
{
    void *result = NULL;
    b32 clear = !(flags & Alloc_NoClear);

    // TODO(michiel): Platform allocation
    if (clear) {
        result = calloc(size, 1);
    } else {
        result = malloc(size);
    }

    return result;
}

internal inline Buffer allocate_buffer(umm size, u32 flags)
{
    Buffer result = {0};
    result.size = size;
    result.data = allocate_array(u8, size, flags);
    if (result.data == NULL)
    {
        result.size = 0;
    }
    return result;
}

internal inline void *
reallocate_size(void *data, umm size)
{
    void *result = NULL;
    result = realloc(data, size);
    return result;
}

internal inline void deallocate(void *mem)
{
    if (mem) {
        free(mem);
    }
}

//
// NOTE(michiel): Arena allocation
//

#define ARENA_ALIGNMENT      8
#define ARENA_ALLOC_MIN_SIZE (1024 * 1024)

internal void
arena_grow(Arena *arena, umm newSize)
{
    ArenaBlock *newBlock = allocate_struct(ArenaBlock, 0);

    newBlock->size = align_up(maximum(newSize, ARENA_ALLOC_MIN_SIZE), ARENA_ALIGNMENT);
    arena->at = newBlock->mem = (u8 *)allocate_size(newBlock->size, 0);

    i_expect(arena->at == align_ptr_down(arena->at, ARENA_ALIGNMENT));
    arena->end = arena->at + newBlock->size;

    newBlock->next = arena->sentinel.next;
    arena->sentinel.next = newBlock;
}

#define arena_allocate_array(a, t, c) (t *)arena_allocate((a), (c) * sizeof(t))
#define arena_allocate_struct(a, t)   (t *)arena_allocate((a), sizeof(t))
internal inline void *
arena_allocate(Arena *arena, umm newSize)
{
    if (newSize > (umm)(arena->end - arena->at)) {
        arena_grow(arena, newSize);
    }
    i_expect(newSize <= (umm)(arena->end - arena->at));

    void *at = arena->at;
    i_expect(at == align_ptr_down(at, ARENA_ALIGNMENT));

    arena->at = (u8 *)align_ptr_up(arena->at + newSize, ARENA_ALIGNMENT);
    i_expect(arena->at <= arena->end);

    return at;
}

internal void
arena_free(Arena *arena)
{
    i_expect(arena->sentinel.size == 0);
    i_expect(arena->sentinel.mem == 0);
    for (ArenaBlock *it = arena->sentinel.next; it; ) {
        ArenaBlock *next = it->next;
        deallocate(it->mem);
        deallocate(it);
        it = next;
    }
    arena->at = 0;
    arena->end = 0;
    arena->sentinel.next = 0;
}

internal inline TempMemory
temporary_memory(Arena *arena)
{
    TempMemory result = {0};
    result.arena = arena;
    result.block = arena->sentinel.next;
    result.origAt = arena->at;
    result.origEnd = arena->end;
    return result;
}

internal inline void
destroy_temporary(TempMemory temp)
{
    Arena *arena = temp.arena;
    i_expect(arena->sentinel.size == 0);
    i_expect(arena->sentinel.mem == 0);
    while ((arena->sentinel.next != temp.block) &&
           (arena->sentinel.next != &arena->sentinel)) {
        ArenaBlock *block = arena->sentinel.next;
        arena->sentinel.next = block->next;
        deallocate(block->mem);
        deallocate(block);
    }
    i_expect(arena->sentinel.next == temp.block);
    arena->at = temp.origAt;
    arena->end = temp.origEnd;
}

//
// NOTE(michiel): Stretchy buffer, see Sean Barrett (stb libs) for credits.
//   Can be used as simple growing array, for fast prototyping.
//   Is meant to be replaced by a defined implementation
//

#define buf__hdr(x)      ((BufHdr *)((u8 *)(x) - offset_of(BufHdr, data)))

#define buf_magic(x)     ((x) ? buf__hdr(x)->magic : BUF_MAGIC)
#define buf_len(x)       ((x) ? buf__hdr(x)->len : 0)
#define buf_cap(x)       ((x) ? buf__hdr(x)->cap : 0)
#define buf_end(x)       ((x) + buf_len(x))
#define buf_last(x)      ((x) ? (x) + buf_len(x) - 1 : 0)
#define buf_sizeof(x)    ((x) ? buf_len(x) * sizeof(*(x)) : 0)

#define buf_free(x)      ((x) ? (free(buf__hdr(x)), (x) = NULL) : 0)
#define buf_fit(x, n)    ((n) <= buf_cap(x) ? 0 : (buf_grow((x), (n)), 0))
#define buf_push(x, v)   (buf_fit((x), 1 + buf_len(x)), (x)[buf__hdr(x)->len++] = (v))
#define buf_pop(x)       (buf_len(x) ? (x)[--buf__hdr(x)->len] : *x) // NOTE(michiel): This can fail!!!!
#define buf_clear(x)     ((x) ? buf__hdr(x)->len = 0 : 0)
#define buf_printf(x, fmt, ...) (buf__printf((void **)&(x), sizeof(*(x)), (fmt), ## __VA_ARGS__))

#define buf_grow(x, n) (buf__grow((void **)&(x), (n), sizeof(*(x))))
internal void
buf__grow(void **bufAddr, umm newCount, umm elemSize)
{
    const void *buf = *bufAddr;
    i_expect(buf_magic(buf) == BUF_MAGIC);
    i_expect(buf_cap(buf) <= (U32_MAX - 1) / 2);
    umm newLen = 2*buf_cap(buf);
    umm newCap = buf_len(buf) + newCount;
    if (newCap < 16) {
        newCap = 16;
    }
    if (newCap < newLen) {
        newCap = newLen;
    }
    i_expect((buf_len(buf) + newCount) <= newCap);
    i_expect(newCap <= (U32_MAX - offset_of(BufHdr, data)) / elemSize);
    umm newSize = offset_of(BufHdr, data) + newCap * elemSize;
    BufHdr *newHdr = (BufHdr *)reallocate_size(buf ? buf__hdr(buf) : 0, newSize);
    i_expect(newHdr);
    if (!buf) {
        newHdr->magic = BUF_MAGIC;
        newHdr->len = 0;
    }
    newHdr->cap = safe_truncate_to_u32(newCap);
    *bufAddr = newHdr->data;
}

internal void
buf__printf(void **bufAddr, umm elemSize, const char *fmt, ...)
{
    void *buf = *bufAddr;
    i_expect(buf_magic(buf) == BUF_MAGIC);
    char *bufEnd = (char *)buf + buf_len(buf)*elemSize;
    va_list args;
    va_start(args, fmt);
    umm cap = buf_cap(buf) - buf_len(buf);
    umm n = 1 + vsnprintf(bufEnd, cap, fmt, args);
    va_end(args);

    if (n > cap) {
        buf__grow(&buf, n + buf_len(buf), elemSize);
        va_start(args, fmt);
        umm newCap = buf_cap(buf) - buf_len(buf);
        bufEnd = (char *)buf + buf_len(buf)*elemSize;
        n = 1 + vsnprintf(bufEnd, newCap, fmt, args);
        i_expect(n <= newCap);
        va_end(args);
    }
    buf__hdr(buf)->len += n - 1;

    *bufAddr = buf;
}

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

