// TODO(michiel): @Remove <stdio> printf and FILE *
#include <stdio.h>
// TODO(michiel): @Remove <stdlib> calloc/realloc/malloc/free stuff
#include <stdlib.h>

#include <stddef.h>    // size_t, ssize_t
#include <stdint.h>    // uint*_t, int*_t
#include <float.h>     // FLT_MIN, FLT_MAX, DBL_MIN, DBL_MAX

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

#if __has_builtin(__builtin_trap)
#define i_expect_simple(expr)   ((expr) ? (void)0 : __builtin_trap())
#define i_expect(expr)          if (!(expr)) { fprintf(stderr, "%s:%d:Expectation failed: '%s'\n", __FILE__, __LINE__, #expr); __builtin_trap(); }
#else
#define i_expect_simple(expr)   ((expr) ? (void)0 : (*(int *)0 = 0))
#define i_expect(expr)          if (!(expr)) { *(int *)0 = 0; }
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

typedef size_t   umm;

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
#define F32_MIN                -FLT_MAX
#define F32_MAX                 FLT_MAX
#define F64_SIGN_MASK           0x8000000000000000
#define F64_EXP_MASK            0x7FF0000000000000
#define F64_FRAC_MASK           0x000FFFFFFFFFFFFF
#define F64_MIN                -DBL_MAX
#define F64_MAX                 DBL_MAX

#define LONG_PI   3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748
#define LONG_E    2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274274663919320030599218174135966290435729003342952605956307381323286279434907632338298807531952510190115738341879307021540891499348841675092447614606680822648001684774118537423454424371075390777449920695517027618386062613314
#define F64_PI                  ((f64)LONG_PI)
#define F32_PI                  ((f32)F64_PI)
#define F64_TAU                 (2.0f * (f64)LONG_PI)
#define F32_TAU                 ((f32)F64_TAU)
#define F64_E                   ((f64)LONG_E)
#define F32_E                   ((f32)F64_E)

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

#define u64_from_ptr(p)         ((u64)(umm)p)
#define ptr_from_u64(u)         ((void *)(umm)u)

#define rad2deg(angle)          (((angle) / LONG_PI) * 180)
#define deg2rad(angle)          (((angle) / 180) * LONG_PI)

#define minimum(a, b)           ((a) < (b) ? (a) : (b))
#define maximum(a, b)           ((a) > (b) ? (a) : (b))
#define clamp(min, x, max)      (maximum(min, minimum(max, x)))

#define kilobytes(kB)           ((kB) * 1024LL)
#define megabytes(MB)           (kilobytes(MB) * 1024LL)
#define gigabytes(GB)           (megabytes(GB) * 1024LL)
#define terabytes(TB)           (gigabytes(TB) * 1024LL)

internal inline u32 safe_truncate_to_u32(u64 value) { i_expect(value <= U32_MAX); return (u32)(value & U32_MAX); }
internal inline u16 safe_truncate_to_u16(u64 value) { i_expect(value <= U16_MAX); return (u16)(value & U16_MAX); }
internal inline u8  safe_truncate_to_u8(u64 value)  { i_expect(value <= U8_MAX);  return (u8)(value & U8_MAX); }
internal inline s32 safe_truncate_to_s32(s64 value) { i_expect(value <= (s64)S32_MAX); i_expect(value >= (s64)S32_MIN); return (s32)value; }
internal inline s16 safe_truncate_to_s16(s64 value) { i_expect(value <= (s64)(s32)S16_MAX); i_expect(value >= (s64)(s32)S16_MIN); return (s16)value; }
internal inline s8  safe_truncate_to_s8(s64 value)  { i_expect(value <= (s64)(s32)(s16)S8_MAX); i_expect(value >= (s64)(s32)(s16)S8_MIN); return (s8)value; }

internal void
copy(umm size, const void *src, void *dst)
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

// NOTE(michiel): Generic buffer (memory data and a size)
 typedef struct Buffer
{
    u32 size;
    u8 *data;
} Buffer;
typedef Buffer String;

typedef struct FileStream
{
    b32 verbose;
    u32 indent;
    // TODO(michiel): Own struct for this (PlatformFile)
    FILE *file;
} FileStream;

// NOTE(michiel): Stretchy buffer header (prepended to keep track of num items etc)
#define BUF_MAGIC 0xB0FFE20F20F78D1E
typedef struct BufHdr
{
    u64 magic;
    u32 cap;
    u32 len;
    u8 data[1];
} BufHdr;

// NOTE(michiel): Memory allocation pool/arena, auto block growth
typedef struct ArenaBlock
{
    struct ArenaBlock *next;
    umm size;
    u8 *mem;
} ArenaBlock;

// TODO(michiel): Random freeing of allocated data
typedef struct Arena
{
    u8 *at;
    u8 *end;
     ArenaBlock sentinel;
} Arena;

// NOTE(michiel): Hashmap/dict implementation to map some key to a value (both must be non-zero)
typedef struct Map
{
    u64 *keys;
    u64 *values;
    u32 len;
    u32 cap;
 } Map;

// NOTE(michiel): String interning
typedef struct Interns
{
    Arena arena;
    Map map;
} Interns;


//
// NOTE(michiel): Allocation
//

typedef enum AllocateFlags
{
    Alloc_NoClear = 0x01,
} AllocateFlags;

#define allocate_array(count, type, ...) (type *)allocate_size(sizeof(type)*count, ## __VA_ARGS__)
#define allocate_struct(type, ...)       (type *)allocate_size(sizeof(type), ## __VA_ARGS__)
internal inline void *allocate_size(u32 size, u32 flags)
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

internal inline Buffer allocate_buffer(u32 size, u32 flags)
{
    Buffer result = {0};
    result.size = size;
    result.data = allocate_array(size, u8, flags);
    if (result.data == NULL)
    {
        result.size = 0;
    }
    return result;
}

internal inline void *
reallocate_size(void *data, u32 size)
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

#define arena_allocate_array(a, c, t) (t *)arena_allocate((a), (c) * sizeof(t))
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

//
// NOTE(michiel): Map
//

// NOTE(michiel): map_(get/put)         => key = ptr, val = ptr
// NOTE(michiel): map_(get/put)_u64     => key = ptr, val = u64
// NOTE(michiel): map_u64_(get/put)     => key = u64, val = ptr
// NOTE(michiel): map_u64_(get/put)_u64 => key = u64, val = u64
#define map_get(map, key)            ptr_from_u64(map_u64_get_u64(map, u64_from_ptr(key)))
#define map_put(map, key, val)       map_u64_put_u64(map, u64_from_ptr(key), u64_from_ptr(val))
#define map_remove(map, key)         map_u64_remove(map, u64_from_ptr(key))
#define map_get_u64(map, key)        map_u64_get_u64(map, u64_from_ptr(key))
#define map_put_u64(map, key, val)   map_u64_put_u64(map, u64_from_ptr(key), val)
#define map_remove_u64(map, key)     map_u64_remove(map, u64_from_ptr(key))
#define map_u64_get(map, key)        ptr_from_u64(map_u64_get_u64(map, key))
#define map_u64_put(map, key, val)   map_u64_put_u64(map, key, u64_from_ptr(val))

#define map_u64_remove_u64           map_u64_remove

internal void
map_free(Map *map)
{
    deallocate(map->keys);
    deallocate(map->values);
}

internal inline u64
map_u64_get_u64(Map *map, u64 key)
{
    u64 result = 0;
    if (map->len > 0) {
        i_expect(is_pow2(map->cap));
        umm hash = (umm)hash_u64(key);
        i_expect(map->len < map->cap);
        for (;;) {
            hash &= map->cap - 1;
            if (map->keys[hash] == key) {
                result = map->values[hash];
                break;
            } else if (!map->keys[hash]) {
                result = 0;
                break;
            }
            ++hash;
        }
    } else {
        result = 0;
    }
    
    return result;
}

internal inline void map_u64_put_u64(Map *map, u64 key, u64 value);

internal void
map_grow(Map *map, umm newCap)
{
    newCap = maximum(newCap, 16);
    Map newMap = {0};
    newMap.keys = allocate_array(newCap, u64, 0);
    newMap.values = allocate_array(newCap, u64, Alloc_NoClear);
    newMap.cap = safe_truncate_to_u32(newCap);
    // NOTE(michiel): Reissue the insertions into our bigger map
    for (u32 mapIndex = 0; mapIndex < map->cap; ++mapIndex) {
        if (map->keys[mapIndex]) {
            map_u64_put_u64(&newMap, map->keys[mapIndex], map->values[mapIndex]);
        }
    }
    map_free(map);
    *map = newMap;
}

internal inline void
map_u64_put_u64(Map *map, u64 key, u64 value)
{
    i_expect(key);
    if ((2 * map->len) >= map->cap) {
        map_grow(map, 2 * map->cap);
    }
    
    i_expect(2 * map->len < map->cap);
    i_expect(is_pow2(map->cap));
    umm hash = (umm)hash_u64(key);
    for (;;) {
        hash &= map->cap - 1;
        if (!map->keys[hash]) {
            ++map->len;
            map->keys[hash] = key;
            map->values[hash] = value;
            break;
        } else if (map->keys[hash] == key) {
            map->values[hash] = value;
            break;
        }
        ++hash;
    }
}

internal inline void
map_u64_remove(Map *map, u64 key)
{
    if (map->len > 0) {
        i_expect(is_pow2(map->cap));
        
        umm hash = (umm)hash_u64(key);
        i_expect(map->len < map->cap);
        
        for (;;) {
            hash &= map->cap - 1;
            if (map->keys[hash] == key) {
                map->keys[hash] = 0;
                map->values[hash] = 0;
                break;
            } else if (!map->keys[hash]) {
                break;
            }
            ++hash;
        }
    }
}

//
// NOTE(michiel): Strings and string interning
//

internal inline b32
is_digit(char n)
{
    return (('0' <= n) && (n <= '9'));
}

internal inline b32
is_alpha(char a)
{
    return ((('a' <= a) && (a <= 'z')) ||
            (('A' <= a) && (a <= 'Z')));
}

internal inline b32
is_alnum(char a)
{
    return is_digit(a) || is_alpha(a);
}

internal inline b32
is_space(char s)
{
    return ((s == ' ') || (s == '\n') || (s == '\r') ||
            (s == '\t') || (s == '\v'));
}

internal inline b32
is_printable(char a)
{
    return (' ' <= a) && (a <= '~');
}

internal inline char
to_lower(char x)
{
    if (('A' <= x) && (x <= 'Z')) {
        return x + ('a' - 'A');
    } else {
        return x;
    }
}

internal inline u32
string_length(const char *cString)
{
    u64 length = 0;
    while (*cString++) {
        ++length;
    }
    return safe_truncate_to_u32(length);
}

internal inline String
string(umm size, const void *data)
{
    String result = {0};
    result.size = size;
    result.data = (u8 *)data;
    return result;
}

#ifdef __cplusplus

internal inline String
string(umm size, const char *data)
{
    // NOTE(michiel): F*ck const ;-)
    return string(size, (void *)data);
}

internal inline String
string(const char *cString)
{
    return string(string_length(cString), cString);
}

internal inline b32
operator ==(String a, String b)
{
    b32 result = (a.size == b.size);
    if (result) {
        if (a.data != b.data) {
            for (u32 index = 0; index < a.size; ++index) {
                if (a.data[index] != b.data[index]) {
                    result = false;
                    break;
                }
            }
        }
    }
    return result;
}

internal inline b32
operator !=(String a, String b)
{
    return !(a == b);
}

internal inline b32
strings_are_equal(umm size, const char *a, String b)
{
    return string(size, a) == b;
}

internal inline b32
strings_are_equal(const char *a, String b)
{
    return string(string_length(a), a) == b;
}

#else

internal inline String
stringc(const char *cString)
{
    return string(string_length(cString), cString);
}

internal inline b32
strings_are_equal(String a, String b)
{
    b32 result = (a.size == b.size);
    if (result) {
        if (a.data != b.data) {
            for (u32 index = 0; index < a.size; ++index) {
                if (a.data[index] != b.data[index]) {
                    result = false;
                    break;
                }
            }
        }
    }
    return result;
}

internal inline b32
strings_are_equal_sc(umm size, const char *a, String b)
{
    return strings_are_equal(string(size, a), b);
}

internal inline b32
strings_are_equal_c(const char *a, String b)
{
    return strings_are_equal(string(string_length(a), a), b);
}

#endif

internal void
str_interns_free(Interns *interns)
{
    arena_free(&interns->arena);
    map_free(&interns->map);
}

// NOTE(michiel): This struct is used internly to speed up the mapping process (->next)
//   and store the string data so every string.data pointer points to the same memory,
//   if they are the same string of course.
typedef struct InternedString
{
    struct InternedString *next;
    u32 size;
    char data[1]; // NOTE(michiel): Placeholder for the string data
} InternedString;

internal InternedString *
str_intern_(Interns *interns, String str)
{
    u64 hash = hash_bytes(str.data, str.size);
    u64 key = hash ? hash : 1;
    InternedString *intern = (InternedString *)map_u64_get(&interns->map, key);
    InternedString *it = intern;
    while (it) {
        String itStr = string(it->size, it->data);
        #ifdef __cplusplus
        if (itStr == str)
            #else
            if (strings_are_equal(itStr, str))
            #endif
            {
            return it;
        }
        it = it->next;
    }
    
    // NOTE(michiel): No matching string found in the map, so add it
    umm newSize = offset_of(InternedString, data) + str.size + 1; // Add one for the 0-term
    InternedString *newInterned = (InternedString *)arena_allocate(&interns->arena, newSize);
    newInterned->next = intern;
    newInterned->size = str.size;
    copy(str.size, str.data, newInterned->data);
    newInterned->data[str.size] = 0;
    map_u64_put(&interns->map, key, newInterned);
    return newInterned;
}

internal InternedString *
str_intern_fmt_(Interns *interns, char *fmt, ...)
{
    static char buffer[4096];
    
    va_list args;
    va_start(args, fmt);
    u64 written = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    i_expect(written <= sizeof(buffer));
    return str_intern_(interns, string(written, buffer));
}

internal String
str_intern(Interns *interns, String str)
{
    InternedString *intStr = str_intern_(interns, str);
    return string(intStr->size, intStr->data);
}

internal String
str_intern_fmt(Interns *interns, char *fmt, ...)
{
    static char buffer[4096];
    
    va_list args;
    va_start(args, fmt);
    u64 written = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    i_expect(written <= sizeof(buffer));
    return str_intern(interns, string(written, buffer));
}

#ifdef __cplusplus

internal InternedString *
str_intern_(Interns *interns, const char *str)
{
    return str_intern_(interns, string(str));
}

internal String
str_intern(Interns *interns, const char *str)
{
    return str_intern(interns, string(str));
}

#else

internal InternedString *
str_intern_c_(Interns *interns, const char *str)
{
    return str_intern_(interns, stringc(str));
}

internal String
str_intern_c(Interns *interns, const char *str)
{
    return str_intern(interns, stringc(str));
}

#endif

#ifdef __cplusplus
#include "common_cpp.h" // Overloaded functions and such things c won't handle
#endif
