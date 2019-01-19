
typedef union v2
{
    struct
    {
        f32 x;
        f32 y;
    };
    f32 e[2];
} v2;

typedef union v2i
{
    struct
    {
        s32 x;
        s32 y;
    };
    s32 e[2];
} v2i;

typedef union v2u
{
    struct
    {
        u32 x;
        u32 y;
    };
    u32 e[2];
} v2u;

typedef union v3
{
    struct
    {
        union
        {
            struct {
                f32 x;
                f32 y;
            };
            v2 xy;
        };
        f32 z;
    };
    f32 e[3];
} v3;

typedef union v4
{
    struct
    {
        union
        {
            struct {
                union
                {
                    struct {
                        f32 x;
                        f32 y;
                    };
                    v2 xy;
                };
                f32 z;
            };
            v3 xyz;
        };
        f32 w;
    };
    struct
    {
        union
        {
            struct {
                f32 r;
                f32 g;
                f32 b;
            };
            v3 rgb;
        };
        f32 a;
    };
    f32 e[4];
} v4;

typedef struct Rectangle2
{
    v2 min;
    v2 max;
} Rectangle2;

typedef struct Rectangle2i
{
    v2i min;
    v2i max;
} Rectangle2i;

typedef struct Rectangle2u
{
    v2u min;
    v2u max;
} Rectangle2u;
