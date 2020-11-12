
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
typedef long double f80;

typedef unsigned long int umm;
typedef signed long int   smm;
compile_expect(sizeof(umm) == sizeof(size_t));

#define enum8(name)             u8
#define enum16(name)            u16
#define enum32(name)            u32
#define enum64(name)            u64

typedef struct DivModU32
{
    u32 div;
    u32 mod;
} DivModU32;

typedef struct DivModS32
{
    s32 div;
    s32 mod;
} DivModS32;

typedef struct DivModU64
{
    u64 div;
    u64 mod;
} DivModU64;

typedef struct DivModS64
{
    s64 div;
    s64 mod;
} DivModS64;

// NOTE(michiel): Generic buffer (memory data and a size)
typedef struct Buffer
{
    umm size;
    u8 *data;
} Buffer;
typedef Buffer String;

struct Stream
{
    Buffer buffer;
    struct Stream *next;
};

// NOTE(michiel): Image buffer (32bit pixels assumed)
typedef struct Image
{
    u32 width;
    u32 height;
    u32 rowStride; // NOTE(michiel): mostly just width, but can be used for subimages
    u32 *pixels;
} Image;

// NOTE(michiel): Image buffer (8bit pixels, grey/alpha), as used in some font instances
typedef struct Image8
{
    u32 width;
    u32 height;
    u32 rowStride;
    u8 *pixels;
} Image8;
