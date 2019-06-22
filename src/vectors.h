#ifdef __cplusplus

//
// NOTE(michiel): V2U
//

internal v2u
V2U(u32 x, u32 y)
{
    v2u result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal v2u
operator -(v2u a)
{
    v2u result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

internal v2u &
operator +=(v2u &a, v2u b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

internal v2u
operator +(v2u a, v2u b)
{
    v2u result = a;
    result += b;
    return result;
}

internal v2u &
operator -=(v2u &a, v2u b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

internal v2u
operator -(v2u a, v2u b)
{
    v2u result = a;
    result -= b;
    return result;
}

internal v2u &
operator &=(v2u &a, u32 b)
{
    a.x &= b;
    a.y &= b;
    return a;
}

internal v2u
operator &(v2u a, u32 b)
{
    v2u result = a;
    result &= b;
    return result;
}

internal v2u &
operator |=(v2u &a, u32 b)
{
    a.x |= b;
    a.y |= b;
    return a;
}

internal v2u
operator |(v2u a, u32 b)
{
    v2u result = a;
    result |= b;
    return result;
}

internal v2u &
operator ^=(v2u &a, u32 b)
{
    a.x ^= b;
    a.y ^= b;
    return a;
}

internal v2u
operator ^(v2u a, u32 b)
{
    v2u result = a;
    result ^= b;
    return result;
}

internal v2u &
operator +=(v2u &a, u32 b)
{
    a.x += b;
    a.y += b;
    return a;
}

internal v2u
operator +(v2u a, u32 b)
{
    v2u result = a;
    result += b;
    return result;
}

internal v2u &
operator -=(v2u &a, u32 b)
{
    a.x -= b;
    a.y -= b;
    return a;
}

internal v2u
operator -(v2u a, u32 b)
{
    v2u result = a;
    result -= b;
    return result;
}

internal v2u &
operator *=(v2u &a, u32 b)
{
    a.x *= b;
    a.y *= b;
    return a;
}

internal v2u
operator *(v2u a, u32 b)
{
    v2u result = a;
    result *= b;
    return result;
}

internal v2u
operator *(u32 a, v2u b)
{
    return b * a;
}

internal v2u &
operator /=(v2u &a, u32 b)
{
    a.x /= b;
    a.y /= b;
    return a;
}

internal v2u
operator /(v2u a, u32 b)
{
    v2u result = a;
    result /= b;
    return result;
}

internal v2u
operator /(u32 a, v2u b)
{
    v2u result;
    result.x = a / b.x;
    result.y = a / b.y;
    return result;
}

internal b32
operator ==(v2u a, v2u b)
{
    b32 result = false;
    result = (a.x == b.x) && (a.y == b.y);
    return result;
}

internal b32
operator !=(v2u a, v2u b)
{
    b32 result = false;
    result = (a.x != b.x) || (a.y != b.y);
    return result;
}

//
// NOTE(michiel): V2S
//

internal v2s
V2S(s32 x, s32 y)
{
    v2s result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal v2s
V2S(v2u u)
{
    v2s result;
    result.x = (u.x < 0x80000000) ? u.x : 0x7FFFFFFF;
    result.y = (u.y < 0x80000000) ? u.y : 0x7FFFFFFF;
    return result;
}

internal v2s
operator -(v2s a)
{
    v2s result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

internal v2s &
operator +=(v2s &a, v2s b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

internal v2s
operator +(v2s a, v2s b)
{
    v2s result = a;
    result += b;
    return result;
}

internal v2s &
operator -=(v2s &a, v2s b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

internal v2s
operator -(v2s a, v2s b)
{
    v2s result = a;
    result -= b;
    return result;
}

internal v2s &
operator &=(v2s &a, u32 b)
{
    a.x &= b;
    a.y &= b;
    return a;
}

internal v2s
operator &(v2s a, u32 b)
{
    v2s result = a;
    result &= b;
    return result;
}

internal v2s &
operator |=(v2s &a, u32 b)
{
    a.x |= b;
    a.y |= b;
    return a;
}

internal v2s
operator |(v2s a, u32 b)
{
    v2s result = a;
    result |= b;
    return result;
}

internal v2s &
operator ^=(v2s &a, u32 b)
{
    a.x ^= b;
    a.y ^= b;
    return a;
}

internal v2s
operator ^(v2s a, u32 b)
{
    v2s result = a;
    result ^= b;
    return result;
}

internal v2s &
operator +=(v2s &a, s32 b)
{
    a.x += b;
    a.y += b;
    return a;
}

internal v2s
operator +(v2s a, s32 b)
{
    v2s result = a;
    result += b;
    return result;
}

internal v2s &
operator -=(v2s &a, s32 b)
{
    a.x -= b;
    a.y -= b;
    return a;
}

internal v2s
operator -(v2s a, s32 b)
{
    v2s result = a;
    result -= b;
    return result;
}

internal v2s &
operator *=(v2s &a, s32 b)
{
    a.x *= b;
    a.y *= b;
    return a;
}

internal v2s
operator *(v2s a, s32 b)
{
    v2s result = a;
    result *= b;
    return result;
}

internal v2s
operator *(s32 a, v2s b)
{
    return b * a;
}

internal v2s &
operator /=(v2s &a, s32 b)
{
    a.x /= b;
    a.y /= b;
    return a;
}

internal v2s
operator /(v2s a, s32 b)
{
    v2s result = a;
    result /= b;
    return result;
}

internal v2s
operator /(s32 a, v2s b)
{
    v2s result;
    result.x = a / b.x;
    result.y = a / b.y;
    return result;
}

internal b32
operator ==(v2s a, v2s b)
{
    b32 result = false;
    result = (a.x == b.x) && (a.y == b.y);
    return result;
}

internal b32
operator !=(v2s a, v2s b)
{
    b32 result = false;
    result = (a.x != b.x) || (a.y != b.y);
    return result;
}

//
// NOTE(michiel): V2
//

internal v2
V2(f32 x, f32 y)
{
    v2 result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal v2
V2(v2s v)
{
    v2 result;
    result.x = (f32)v.x;
    result.y = (f32)v.y;
    return result;
}

internal v2
V2(v2u v)
{
    v2 result;
    result.x = (f32)v.x;
    result.y = (f32)v.y;
    return result;
}

internal v2
polar_to_cartesian(f32 r, f32 theta)
{
    v2 result;
    result.x = r * cos(theta);
    result.y = r * sin(theta);
    return result;
}

internal v2
operator -(v2 a)
{
    v2 result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

internal v2 &
operator +=(v2 &a, v2 b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

internal v2
operator +(v2 a, v2 b)
{
    v2 result = a;
    result += b;
    return result;
}

internal v2 &
operator -=(v2 &a, v2 b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

internal v2
operator -(v2 a, v2 b)
{
    v2 result = a;
    result -= b;
    return result;
}

internal v2 &
operator +=(v2 &a, f32 b)
{
    a.x += b;
    a.y += b;
    return a;
}

internal v2
operator +(v2 a, f32 b)
{
    v2 result = a;
    result += b;
    return result;
}

internal v2 &
operator -=(v2 &a, f32 b)
{
    a.x -= b;
    a.y -= b;
    return a;
}

internal v2
operator -(v2 a, f32 b)
{
    v2 result = a;
    result -= b;
    return result;
}

internal v2 &
operator *=(v2 &a, f32 b)
{
    a.x *= b;
    a.y *= b;
    return a;
}

internal v2
operator *(v2 a, f32 b)
{
    v2 result = a;
    result *= b;
    return result;
}

internal v2
operator *(f32 a, v2 b)
{
    return b * a;
}

internal v2 &
operator /=(v2 &a, f32 b)
{
    a *= 1.0f / b;
    return a;
}

internal v2
operator /(v2 a, f32 b)
{
    v2 result = a;
    result /= b;
    return result;
}

internal v2
operator /(f32 a, v2 b)
{
    v2 result;
    result.x = a / b.x;
    result.y = a / b.y;
    return result;
}

internal b32
operator ==(v2 a, v2 b)
{
    b32 result = false;
    result = (a.x == b.x) && (a.y == b.y);
    return result;
}

internal b32
operator !=(v2 a, v2 b)
{
    b32 result = false;
    result = (a.x != b.x) || (a.y != b.y);
    return result;
}

internal v2
clamp01(v2 a)
{
    v2 result;
    result.x = clamp01(a.x);
    result.y = clamp01(a.y);
    return result;
}

internal v2
hadamard(v2 a, v2 b)
{
    v2 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

internal f32
dot(v2 a, v2 b)
{
    f32 result;
    result = a.x * b.x + a.y * b.y;
    return result;
}

internal f32
length_squared(v2 a)
{
    f32 result = dot(a, a);
    return result;
}

internal f32
length(v2 a)
{
    f32 result = length_squared(a);
    result = square_root(result);
    return result;
}

internal v2
normalize_len(v2 a, f32 len)
{
    v2 result = {};
    if (len != 0.0f)
    {
        result = a / len;
    }
    return result;
}

internal v2
normalize(v2 a)
{
    v2 result = normalize_len(a, length(a));
    return result;
}

internal v2
set_length(v2 a, f32 length)
{
    v2 result = normalize(a);
    result *= length;
    return result;
}

internal v2
direction(v2 from, v2 to)
{
    v2 result = to - from;
    return result;
}

internal v2
direction_unit(v2 from, v2 to)
{
    v2 result = normalize(to - from);
    return result;
}

internal v2
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

internal v3u
V3U(u32 x, u32 y, u32 z)
{
    v3u result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    
    return result;
}

internal v3u
V3U(v2u xy, u32 z)
{
    v3u result;
    
    result.xy = xy;
    result.z = z;
    
    return result;
}

internal v3u
operator -(v3u a)
{
    v3u result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    return result;
}

internal v3u &
operator +=(v3u &a, v3u b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

internal v3u
operator +(v3u a, v3u b)
{
    v3u result = a;
    result += b;
    return result;
}

internal v3u &
operator -=(v3u &a, v3u b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

internal v3u
operator -(v3u a, v3u b)
{
    v3u result = a;
    result -= b;
    return result;
}

internal v3u &
operator &=(v3u &a, u32 b)
{
    a.x &= b;
    a.y &= b;
    a.z &= b;
    return a;
}

internal v3u
operator &(v3u a, u32 b)
{
    v3u result = a;
    result &= b;
    return result;
}

internal v3u &
operator |=(v3u &a, u32 b)
{
    a.x |= b;
    a.y |= b;
    a.z |= b;
    return a;
}

internal v3u
operator |(v3u a, u32 b)
{
    v3u result = a;
    result |= b;
    return result;
}

internal v3u &
operator ^=(v3u &a, u32 b)
{
    a.x ^= b;
    a.y ^= b;
    a.z ^= b;
    return a;
}

internal v3u
operator ^(v3u a, u32 b)
{
    v3u result = a;
    result ^= b;
    return result;
}

internal v3u &
operator +=(v3u &a, u32 b)
{
    a.x += b;
    a.y += b;
    a.z += b;
    return a;
}

internal v3u
operator +(v3u a, u32 b)
{
    v3u result = a;
    result += b;
    return result;
}

internal v3u &
operator -=(v3u &a, u32 b)
{
    a.x -= b;
    a.y -= b;
    a.z -= b;
    return a;
}

internal v3u
operator -(v3u a, u32 b)
{
    v3u result = a;
    result -= b;
    return result;
}

internal v3u &
operator *=(v3u &a, u32 b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    return a;
}

internal v3u
operator *(v3u a, u32 b)
{
    v3u result = a;
    result *= b;
    return result;
}

internal v3u
operator *(u32 a, v3u b)
{
    return b * a;
}

internal v3u &
operator /=(v3u &a, u32 b)
{
    a.x /= b;
    a.y /= b;
    a.z /= b;
    return a;
}

internal v3u
operator /(v3u a, u32 b)
{
    v3u result = a;
    result /= b;
    return result;
}

internal b32
operator ==(v3u a, v3u b)
{
    b32 result = false;
    result = (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
    return result;
}

internal b32
operator !=(v3u a, v3u b)
{
    b32 result = false;
    result = (a.x != b.x) || (a.y != b.y) || (a.z != b.z);
    return result;
}

//
// NOTE(michiel): V3
//

internal v3
V3(f32 x, f32 y, f32 z)
{
    v3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

internal v3
V3(v2 xy, f32 z)
{
    v3 result;
    
    result.xy = xy;
    result.z = z;
    
    return result;
}

internal v3
operator -(v3 a)
{
    v3 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    return result;
}

internal v3 &
operator +=(v3 &a, v3 b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

internal v3
operator +(v3 a, v3 b)
{
    v3 result = a;
    result += b;
    return result;
}

internal v3 &
operator -=(v3 &a, v3 b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

internal v3
operator -(v3 a, v3 b)
{
    v3 result = a;
    result -= b;
    return result;
}

internal v3 &
operator +=(v3 &a, f32 b)
{
    a.x += b;
    a.y += b;
    a.z += b;
    return a;
}

internal v3
operator +(v3 a, f32 b)
{
    v3 result = a;
    result += b;
    return result;
}

internal v3 &
operator -=(v3 &a, f32 b)
{
    a.x -= b;
    a.y -= b;
    a.z -= b;
    return a;
}

internal v3
operator -(v3 a, f32 b)
{
    v3 result = a;
    result -= b;
    return result;
}

internal v3 &
operator *=(v3 &a, f32 b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    return a;
}

internal v3
operator *(v3 a, f32 b)
{
    v3 result = a;
    result *= b;
    return result;
}

internal v3
operator *(f32 a, v3 b)
{
    return b * a;
}

internal v3 &
operator /=(v3 &a, f32 b)
{
    a *= 1.0f / b;
    return a;
}

internal v3
operator /(v3 a, f32 b)
{
    v3 result = a;
    result /= b;
    return result;
}

internal b32
operator ==(v3 a, v3 b)
{
    b32 result = false;
    result = (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
    return result;
}

internal b32
operator !=(v3 a, v3 b)
{
    b32 result = false;
    result = (a.x != b.x) || (a.y != b.y) || (a.z != b.z);
    return result;
}

internal v3
hadamard(v3 a, v3 b)
{
    v3 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

internal v3
cross(v3 a, v3 b)
{
    v3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

internal f32
dot(v3 a, v3 b)
{
    f32 result;
    result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

internal f32
length_squared(v3 a)
{
    f32 result = dot(a, a);
    return result;
}

internal f32
length(v3 a)
{
    f32 result = length_squared(a);
    result = square_root(result);
    return result;
}

internal v3
normalize_or_zero(v3 a, f32 lengthSquared)
{
    v3 result = {};
    if (lengthSquared > square(0.0001f))
    {
        result = a * (1.0f / square_root(lengthSquared));
    }
    return result;
}

internal v3
normalize_or_zero(v3 a)
{
    v3 result = normalize_or_zero(a, length_squared(a));
    return result;
}

internal v3
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

internal v4
V4(f32 x, f32 y, f32 z, f32 w)
{
    v4 result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    
    return result;
}

internal v4
V4(v2 xy, f32 z, f32 w)
{
    v4 result;
    
    result.xy = xy;
    result.z = z;
    result.w = w;
    
    return result;
}

internal v4
V4(v2 xy, v2 zw)
{
    v4 result;
    
    result.xy = xy;
    result.zw = zw;
    
    return result;
}

internal v4
V4(v3 xyz, f32 w)
{
    v4 result;
    
    result.xyz = xyz;
    result.w = w;
    
    return result;
}

internal v4
operator -(v4 a)
{
    v4 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;
    return result;
}

internal v4 &
operator +=(v4 &a, v4 b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    a.w += b.w;
    return a;
}

internal v4
operator +(v4 a, v4 b)
{
    v4 result = a;
    result += b;
    return result;
}

internal v4 &
operator -=(v4 &a, v4 b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    a.w -= b.w;
    return a;
}

internal v4
operator -(v4 a, v4 b)
{
    v4 result = a;
    result -= b;
    return result;
}

internal v4 &
operator +=(v4 &a, f32 b)
{
    a.x += b;
    a.y += b;
    a.z += b;
    a.w += b;
    return a;
}

internal v4
operator +(v4 a, f32 b)
{
    v4 result = a;
    result += b;
    return result;
}

internal v4 &
operator -=(v4 &a, f32 b)
{
    a.x -= b;
    a.y -= b;
    a.z -= b;
    a.w -= b;
    return a;
}

internal v4
operator -(v4 a, f32 b)
{
    v4 result = a;
    result -= b;
    return result;
}

internal v4 &
operator *=(v4 &a, f32 b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    a.w *= b;
    return a;
}

internal v4
operator *(v4 a, f32 b)
{
    v4 result = a;
    result *= b;
    return result;
}

internal v4
operator *(f32 a, v4 b)
{
    return b * a;
}

internal v4 &
operator /=(v4 &a, f32 b)
{
    a *= 1.0f / b;
    return a;
}

internal v4
operator /(v4 a, f32 b)
{
    v4 result = a;
    result /= b;
    return result;
}

internal b32
operator ==(v4 a, v4 b)
{
    b32 result = false;
    result = (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
    return result;
}

internal b32
operator !=(v4 a, v4 b)
{
    b32 result = false;
    result = (a.x != b.x) || (a.y != b.y) || (a.z != b.z) || (a.w != b.w);
    return result;
}

internal f32
dot(v4 a, v4 b)
{
    f32 result;
    result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return result;
}

internal f32
length_squared(v4 a)
{
    f32 result = dot(a, a);
    return result;
}

internal f32
length(v4 a)
{
    f32 result = length_squared(a);
    result = square_root(result);
    return result;
}

internal v4
normalize(v4 a, f32 len)
{
    v4 result = {};
    if (len != 0.0f)
    {
        result = a / len;
    }
    return result;
}

internal v4
normalize(v4 a)
{
    v4 result = normalize(a, length(a));
    return result;
}

internal v4
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
//

internal Rectangle2u
rect_from_dim(u32 x, u32 y, u32 w, u32 h)
{
    Rectangle2u result = {0};
    result.min.x = x;
    result.min.y = y;
    result.max.x = x + w;
    result.max.y = y + h;
    return result;
}

internal v2u
get_dim(Rectangle2u rect)
{
    v2u result = rect.max - rect.min;
    return result;
}

//
// NOTE(michiel): Rectangle2s
//

inline Rectangle2s
rect_min_dim(s32 minX, s32 minY, s32 dimX, s32 dimY)
{
    Rectangle2s result;
    
    result.min.x = minX;
    result.min.y = minY;
    result.max.x = minX + dimX;
    result.max.y = minY + dimY;
    
    return result;
}

inline Rectangle2s
rect_min_max(s32 minX, s32 minY, s32 maxX, s32 maxY)
{
    Rectangle2s result;
    
    result.min.x = minX;
    result.min.y = minY;
    result.max.x = maxX;
    result.max.y = maxY;
    
    return result;
}

#if 0
inline Rectangle2s
rect_center_dim(v2s center, v2s dim)
{
    Rectangle2s result;
    
    result.min = center - dim / 2;
    result.max = center + dim / 2;
    
    return result;
}
#endif

internal v2s
get_dim(Rectangle2s rect)
{
    v2s result = rect.max - rect.min;
    return result;
}

internal s32
get_width(Rectangle2s a)
{
    s32 result = a.max.x - a.min.x;
    return result;
}

internal s32
get_height(Rectangle2s a)
{
    s32 result = a.max.y - a.min.y;
    return result;
}

//
// NOTE(michiel): Rectangle2
//

inline Rectangle2
inverted_infinity_rectangle2(void)
{
    Rectangle2 result;
    
    result.min.x = F32_MAX;
    result.min.y = F32_MAX;
    result.max.x = F32_MIN;
    result.max.y = F32_MIN;
    
    return result;
}

inline Rectangle2
rect_min_dim(f32 minX, f32 minY, f32 dimX, f32 dimY)
{
    Rectangle2 result;
    
    result.min.x = minX;
    result.min.y = minY;
    result.max.x = minX + dimX;
    result.max.y = minY + dimY;
    
    return result;
}

inline Rectangle2
rect_min_dim(v2 min, v2 dim)
{
    Rectangle2 result;
    
    result.min = min;
    result.max = min + dim;
    
    return result;
}

inline Rectangle2
rect_center_dim(v2 center, v2 dim)
{
    Rectangle2 result;
    
    result.min = center - 0.5f * dim;
    result.max = center + 0.5f * dim;
    
    return result;
}

inline f32
get_width(Rectangle2 a)
{
    f32 result = a.max.x - a.min.x;
    return result;
}

inline f32
get_height(Rectangle2 a)
{
    f32 result = a.max.y - a.min.y;
    return result;
}

inline v2
get_dim(Rectangle2 a)
{
    v2 result = a.max - a.min;
    return result;
}

inline Rectangle2
rect_grow(Rectangle2 a, v2 border)
{
    Rectangle2 result;
    
    result.min.x = a.min.x - border.x;
    result.max.x = a.max.x + border.x;
    result.min.y = a.min.y - border.y;
    result.max.y = a.max.y + border.y;
    
    return result;
}

inline Rectangle2
rect_union(Rectangle2 a, Rectangle2 b)
{
    Rectangle2 result;
    
    result.min.x = minimum(a.min.x, b.min.x);
    result.min.y = minimum(a.min.y, b.min.y);
    result.max.x = maximum(a.max.x, b.max.x);
    result.max.y = maximum(a.max.y, b.max.y);
    
    return result;
}

#endif
