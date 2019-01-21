typedef union v2 {
    struct {
        f32 x;
        f32 y;
    };
    f32 e[2];
} v2;

typedef union v2s {
    struct {
        s32 x;
        s32 y;
    };
    s32 e[2];
} v2s;

typedef union v2u {
    struct {
        u32 x;
        u32 y;
    };
    u32 e[2];
} v2u;

typedef union v3 {
    struct {
        union {
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

typedef union v3s {
    struct {
        union {
            struct {
                s32 x;
                s32 y;
            };
            v2s xy;
        };
        s32 z;
    };
    s32 e[3];
} v3s;

typedef union v3u {
    struct {
        union {
            struct {
                u32 x;
                u32 y;
            };
            v2u xy;
        };
        u32 z;
    };
    u32 e[3];
} v3u;

typedef union v4 {
    struct {
        union {
            struct {
                union {
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
    struct {
        union {
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

typedef union v4s {
    struct {
        union {
            struct {
                union {
                    struct {
                        s32 x;
                        s32 y;
                    };
                    v2s xy;
                };
                s32 z;
            };
            v3s xyz;
        };
        s32 w;
    };
    struct {
        union {
            struct {
                s32 r;
                s32 g;
                s32 b;
            };
            v3s rgb;
        };
        s32 a;
    };
    s32 e[4];
} v4s;

typedef union v4u {
    struct {
        union {
            struct {
                union {
                    struct {
                        u32 x;
                        u32 y;
                    };
                    v2u xy;
                };
                u32 z;
            };
            v3u xyz;
        };
        u32 w;
    };
    struct {
        union {
            struct {
                u32 r;
                u32 g;
                u32 b;
            };
            v3u rgb;
        };
        u32 a;
    };
    u32 e[4];
} v4u;

typedef struct Rectangle2
{
    v2 min;
    v2 max;
} Rectangle2;

typedef struct Rectangle2s
{
    v2s min;
    v2s max;
} Rectangle2s;

typedef struct Rectangle2u
{
    v2u min;
    v2u max;
} Rectangle2u;

#ifdef __cplusplus

//
// NOTE(michiel): V2U
//

internal inline v2u
V2U(u32 x, u32 y)
{
    v2u result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal inline v2u
operator -(v2u a)
{
    v2u result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

internal inline v2u &
operator +=(v2u &a, v2u b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

internal inline v2u
operator +(v2u a, v2u b)
{
    v2u result = a;
    result += b;
    return result;
}

internal inline v2u &
operator -=(v2u &a, v2u b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

internal inline v2u
operator -(v2u a, v2u b)
{
    v2u result = a;
    result -= b;
    return result;
}

internal inline v2u &
operator &=(v2u &a, u32 b)
{
    a.x &= b;
    a.y &= b;
    return a;
}

internal inline v2u
operator &(v2u a, u32 b)
{
    v2u result = a;
    result &= b;
    return result;
}

internal inline v2u &
operator |=(v2u &a, u32 b)
{
    a.x |= b;
    a.y |= b;
    return a;
}

internal inline v2u
operator |(v2u a, u32 b)
{
    v2u result = a;
    result |= b;
    return result;
}

internal inline v2u &
operator ^=(v2u &a, u32 b)
{
    a.x ^= b;
    a.y ^= b;
    return a;
}

internal inline v2u
operator ^(v2u a, u32 b)
{
    v2u result = a;
    result ^= b;
    return result;
}

internal inline v2u &
operator +=(v2u &a, u32 b)
{
    a.x += b;
    a.y += b;
    return a;
}

internal inline v2u
operator +(v2u a, u32 b)
{
    v2u result = a;
    result += b;
    return result;
}

internal inline v2u &
operator -=(v2u &a, u32 b)
{
    a.x -= b;
    a.y -= b;
    return a;
}

internal inline v2u
operator -(v2u a, u32 b)
{
    v2u result = a;
    result -= b;
    return result;
}

internal inline v2u &
operator *=(v2u &a, u32 b)
{
    a.x *= b;
    a.y *= b;
    return a;
}

internal inline v2u
operator *(v2u a, u32 b)
{
    v2u result = a;
    result *= b;
    return result;
}

internal inline v2u
operator *(u32 a, v2u b)
{
    return b * a;
}

internal inline v2u &
operator /=(v2u &a, u32 b)
{
    a.x /= b;
    a.y /= b;
    return a;
}

internal inline v2u
operator /(v2u a, u32 b)
{
    v2u result = a;
    result /= b;
    return result;
}

internal inline v2u
operator /(u32 a, v2u b)
{
    v2u result;
    result.x = a / b.x;
    result.y = a / b.y;
    return result;
}

//
// NOTE(michiel): V2S
//

internal inline v2s
V2S(s32 x, s32 y)
{
    v2s result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal inline v2s
V2S(v2u u)
{
    v2s result;
    result.x = (u.x < 0x80000000) ? u.x : 0x7FFFFFFF;
    result.y = (u.y < 0x80000000) ? u.y : 0x7FFFFFFF;
    return result;
}

internal inline v2s
operator -(v2s a)
{
    v2s result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

internal inline v2s &
operator +=(v2s &a, v2s b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

internal inline v2s
operator +(v2s a, v2s b)
{
    v2s result = a;
    result += b;
    return result;
}

internal inline v2s &
operator -=(v2s &a, v2s b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

internal inline v2s
operator -(v2s a, v2s b)
{
    v2s result = a;
    result -= b;
    return result;
}

internal inline v2s &
operator &=(v2s &a, u32 b)
{
    a.x &= b;
    a.y &= b;
    return a;
}

internal inline v2s
operator &(v2s a, u32 b)
{
    v2s result = a;
    result &= b;
    return result;
}

internal inline v2s &
operator |=(v2s &a, u32 b)
{
    a.x |= b;
    a.y |= b;
    return a;
}

internal inline v2s
operator |(v2s a, u32 b)
{
    v2s result = a;
    result |= b;
    return result;
}

internal inline v2s &
operator ^=(v2s &a, u32 b)
{
    a.x ^= b;
    a.y ^= b;
    return a;
}

internal inline v2s
operator ^(v2s a, u32 b)
{
    v2s result = a;
    result ^= b;
    return result;
}

internal inline v2s &
operator +=(v2s &a, s32 b)
{
    a.x += b;
    a.y += b;
    return a;
}

internal inline v2s
operator +(v2s a, s32 b)
{
    v2s result = a;
    result += b;
    return result;
}

internal inline v2s &
operator -=(v2s &a, s32 b)
{
    a.x -= b;
    a.y -= b;
    return a;
}

internal inline v2s
operator -(v2s a, s32 b)
{
    v2s result = a;
    result -= b;
    return result;
}

internal inline v2s &
operator *=(v2s &a, s32 b)
{
    a.x *= b;
    a.y *= b;
    return a;
}

internal inline v2s
operator *(v2s a, s32 b)
{
    v2s result = a;
    result *= b;
    return result;
}

internal inline v2s
operator *(s32 a, v2s b)
{
    return b * a;
}

internal inline v2s &
operator /=(v2s &a, s32 b)
{
    a.x /= b;
    a.y /= b;
    return a;
}

internal inline v2s
operator /(v2s a, s32 b)
{
    v2s result = a;
    result /= b;
    return result;
}

internal inline v2s
operator /(s32 a, v2s b)
{
    v2s result;
    result.x = a / b.x;
    result.y = a / b.y;
    return result;
}

//
// NOTE(michiel): V2
//

internal inline v2
V2(f32 x, f32 y)
{
    v2 result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal inline v2
V2(v2s v)
{
    v2 result;
    result.x = (f32)v.x;
    result.y = (f32)v.y;
    return result;
}

internal inline v2
V2(v2u v)
{
    v2 result;
    result.x = (f32)v.x;
    result.y = (f32)v.y;
    return result;
}

internal inline v2
polar_to_cartesian(f32 r, f32 theta)
{
    v2 result;
    result.x = r * cos(theta);
    result.y = r * sin(theta);
    return result;
}

internal inline v2
operator -(v2 a)
{
    v2 result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

internal inline v2 &
operator +=(v2 &a, v2 b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

internal inline v2
operator +(v2 a, v2 b)
{
    v2 result = a;
    result += b;
    return result;
}

internal inline v2 &
operator -=(v2 &a, v2 b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

internal inline v2
operator -(v2 a, v2 b)
{
    v2 result = a;
    result -= b;
    return result;
}

internal inline v2 &
operator +=(v2 &a, f32 b)
{
    a.x += b;
    a.y += b;
    return a;
}

internal inline v2
operator +(v2 a, f32 b)
{
    v2 result = a;
    result += b;
    return result;
}

internal inline v2 &
operator -=(v2 &a, f32 b)
{
    a.x -= b;
    a.y -= b;
    return a;
}

internal inline v2
operator -(v2 a, f32 b)
{
    v2 result = a;
    result -= b;
    return result;
}

internal inline v2 &
operator *=(v2 &a, f32 b)
{
    a.x *= b;
    a.y *= b;
    return a;
}

internal inline v2
operator *(v2 a, f32 b)
{
    v2 result = a;
    result *= b;
    return result;
}

internal inline v2
operator *(f32 a, v2 b)
{
    return b * a;
}

internal inline v2 &
operator /=(v2 &a, f32 b)
{
    a *= 1.0f / b;
    return a;
}

internal inline v2
operator /(v2 a, f32 b)
{
    v2 result = a;
    result /= b;
    return result;
}

internal inline v2
operator /(f32 a, v2 b)
{
    v2 result;
    result.x = a / b.x;
    result.y = a / b.y;
    return result;
}

internal inline v2
clamp01(v2 a)
{
    v2 result;
    result.x = clamp01(a.x);
    result.y = clamp01(a.y);
    return result;
}

internal inline v2
hadamard(v2 a, v2 b)
{
     v2 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

internal inline f32
dot(v2 a, v2 b)
{
    f32 result;
    result = a.x * b.x + a.y * b.y;
    return result;
}

internal inline f32
length_squared(v2 a)
{
    f32 result = dot(a, a);
    return result;
}

internal inline f32
length(v2 a)
{
    f32 result = length_squared(a);
    result = sqrt(result);
    return result;
}

internal inline v2
normalize_len(v2 a, f32 len)
{
    v2 result = {};
    if (len != 0.0f)
    {
        result = a / len;
    }
    return result;
}

internal inline v2
normalize(v2 a)
{
    v2 result = normalize_len(a, length(a));
    return result;
}

internal inline v2
set_length(v2 a, f32 length)
{
    v2 result = normalize(a);
    result *= length;
    return result;
}

internal inline v2
direction(v2 from, v2 to)
{
    v2 result = to - from;
    return result;
}

internal inline v2
direction_unit(v2 from, v2 to)
{
    v2 result = normalize(to - from);
    return result;
}

internal inline v2
rotate(v2 a, v2 rotation)
{
    v2 result = {};
    
    result.x = a.x * rotation.x - a.y * rotation.y;
    result.y = a.x * rotation.y + a.y * rotation.x;
    
    return result;
}

//
// NOTE(michiel): V3U
//

internal inline v3u
V3U(u32 x, u32 y, u32 z)
{
    v3u result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    
    return result;
}

internal inline v3u
operator -(v3u a)
{
    v3u result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    return result;
}

internal inline v3u &
operator +=(v3u &a, v3u b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

internal inline v3u
operator +(v3u a, v3u b)
{
    v3u result = a;
    result += b;
    return result;
}

internal inline v3u &
operator -=(v3u &a, v3u b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

internal inline v3u
operator -(v3u a, v3u b)
{
    v3u result = a;
    result -= b;
    return result;
}

internal inline v3u &
operator &=(v3u &a, u32 b)
{
    a.x &= b;
    a.y &= b;
    a.z &= b;
    return a;
}

internal inline v3u
operator &(v3u a, u32 b)
{
    v3u result = a;
    result &= b;
    return result;
}

internal inline v3u &
operator |=(v3u &a, u32 b)
{
    a.x |= b;
    a.y |= b;
    a.z |= b;
    return a;
}

internal inline v3u
operator |(v3u a, u32 b)
{
    v3u result = a;
    result |= b;
    return result;
}

internal inline v3u &
operator ^=(v3u &a, u32 b)
{
    a.x ^= b;
    a.y ^= b;
    a.z ^= b;
    return a;
}

internal inline v3u
operator ^(v3u a, u32 b)
{
    v3u result = a;
    result ^= b;
    return result;
}

internal inline v3u &
operator +=(v3u &a, u32 b)
{
    a.x += b;
    a.y += b;
    a.z += b;
    return a;
}

internal inline v3u
operator +(v3u a, u32 b)
{
    v3u result = a;
    result += b;
    return result;
}

internal inline v3u &
operator -=(v3u &a, u32 b)
{
    a.x -= b;
    a.y -= b;
    a.z -= b;
    return a;
}

internal inline v3u
operator -(v3u a, u32 b)
{
    v3u result = a;
    result -= b;
    return result;
}

internal inline v3u &
operator *=(v3u &a, u32 b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    return a;
}

internal inline v3u
operator *(v3u a, u32 b)
{
    v3u result = a;
    result *= b;
    return result;
}

internal inline v3u
operator *(u32 a, v3u b)
{
    return b * a;
}

internal inline v3u &
operator /=(v3u &a, u32 b)
{
    a.x /= b;
    a.y /= b;
    a.z /= b;
    return a;
}

internal inline v3u
operator /(v3u a, u32 b)
{
    v3u result = a;
    result /= b;
    return result;
}

//
// NOTE(michiel): V3
//

internal inline v3
V3(f32 x, f32 y, f32 z)
{
    v3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

internal inline v3
operator -(v3 a)
{
    v3 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    return result;
}

internal inline v3 &
operator +=(v3 &a, v3 b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

internal inline v3
operator +(v3 a, v3 b)
{
    v3 result = a;
    result += b;
    return result;
}

internal inline v3 &
operator -=(v3 &a, v3 b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

internal inline v3
operator -(v3 a, v3 b)
{
    v3 result = a;
    result -= b;
    return result;
}

internal inline v3 &
operator +=(v3 &a, f32 b)
{
    a.x += b;
    a.y += b;
    a.z += b;
    return a;
}

internal inline v3
operator +(v3 a, f32 b)
{
    v3 result = a;
    result += b;
    return result;
}

internal inline v3 &
operator -=(v3 &a, f32 b)
{
    a.x -= b;
    a.y -= b;
    a.z -= b;
    return a;
}

internal inline v3
operator -(v3 a, f32 b)
{
    v3 result = a;
    result -= b;
    return result;
}

internal inline v3 &
operator *=(v3 &a, f32 b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    return a;
}

internal inline v3
operator *(v3 a, f32 b)
{
    v3 result = a;
    result *= b;
    return result;
}

internal inline v3
operator *(f32 a, v3 b)
{
    return b * a;
}

internal inline v3 &
operator /=(v3 &a, f32 b)
{
    a *= 1.0f / b;
    return a;
}

internal inline v3
operator /(v3 a, f32 b)
{
    v3 result = a;
    result /= b;
    return result;
}

internal inline f32
dot(v3 a, v3 b)
{
    f32 result;
    result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

internal inline f32
length_squared(v3 a)
{
    f32 result = dot(a, a);
    return result;
}

internal inline f32
length(v3 a)
{
    f32 result = length_squared(a);
    result = sqrt(result);
    return result;
}

internal inline v3
normalize(v3 a, f32 len)
{
    v3 result = {};
    if (len != 0.0f)
    {
        result = a / len;
    }
    return result;
}

internal inline v3
normalize(v3 a)
{
    v3 result = normalize(a, length(a));
    return result;
}

internal inline v3
clamp01(v3 a)
{
    v3 result;
    result.x = clamp01(a.x);
    result.y = clamp01(a.y);
    result.z = clamp01(a.z);
    return result;
}

//
// NOTE(michiel): V4
//

internal inline v4
V4(f32 x, f32 y, f32 z, f32 w)
{
    v4 result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    
    return result;
}

internal inline v4
operator -(v4 a)
{
    v4 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;
    return result;
}

internal inline v4 &
operator +=(v4 &a, v4 b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    a.w += b.w;
    return a;
}

internal inline v4
operator +(v4 a, v4 b)
{
    v4 result = a;
    result += b;
    return result;
}

internal inline v4 &
operator -=(v4 &a, v4 b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    a.w -= b.w;
    return a;
}

internal inline v4
operator -(v4 a, v4 b)
{
    v4 result = a;
    result -= b;
    return result;
}

internal inline v4 &
operator +=(v4 &a, f32 b)
{
    a.x += b;
    a.y += b;
    a.z += b;
    a.w += b;
    return a;
}

internal inline v4
operator +(v4 a, f32 b)
{
    v4 result = a;
    result += b;
    return result;
}

internal inline v4 &
operator -=(v4 &a, f32 b)
{
    a.x -= b;
    a.y -= b;
    a.z -= b;
    a.w -= b;
    return a;
}

internal inline v4
operator -(v4 a, f32 b)
{
    v4 result = a;
    result -= b;
    return result;
}

internal inline v4 &
operator *=(v4 &a, f32 b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    a.w *= b;
    return a;
}

internal inline v4
operator *(v4 a, f32 b)
{
    v4 result = a;
    result *= b;
    return result;
}

internal inline v4
operator *(f32 a, v4 b)
{
    return b * a;
}

internal inline v4 &
operator /=(v4 &a, f32 b)
{
    a *= 1.0f / b;
    return a;
}

internal inline v4
operator /(v4 a, f32 b)
{
    v4 result = a;
    result /= b;
    return result;
}

internal inline f32
dot(v4 a, v4 b)
{
    f32 result;
    result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return result;
}

internal inline f32
length_squared(v4 a)
{
    f32 result = dot(a, a);
    return result;
}

internal inline f32
length(v4 a)
{
    f32 result = length_squared(a);
    result = sqrt(result);
    return result;
}

internal inline v4
normalize(v4 a, f32 len)
{
    v4 result = {};
    if (len != 0.0f)
    {
        result = a / len;
    }
    return result;
}

internal inline v4
normalize(v4 a)
{
    v4 result = normalize(a, length(a));
    return result;
}

internal inline v4
clamp01(v4 a)
{
    v4 result;
    result.x = clamp01(a.x);
    result.y = clamp01(a.y);
    result.z = clamp01(a.z);
    result.w = clamp01(a.w);
    return result;
}

//
// NOTE(michiel): Rectangle2u

internal inline Rectangle2u
rect_from_dim(u32 x, u32 y, u32 w, u32 h)
{
    Rectangle2u result = {0};
    result.min.x = x;
    result.min.y = y;
    result.max.x = x + w;
    result.max.y = y + h;
    return result;
}

internal inline v2u
get_dim(Rectangle2u rect)
{
    v2u result = rect.max - rect.min;
    return result;
}

#endif
