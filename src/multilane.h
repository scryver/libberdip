#define MULTILANE_SHUFFLE_MASK(a, b, c, d) (((d) << 6) | ((c) << 4) | ((b) << 2) | (a))

union f32_4x
{
    __m128  m;
    __m128i mi;
    f32     e[4];
    u32     u[4];
};

union f64_2x
{
    __m128  m;
    __m128i mi;
    f64     e[2];
    u64     u[2];
};

union v2_4x
{
    struct {
        f32_4x x;
        f32_4x y;
    };
    f32_4x e[2];
};

union v3_4x
{
    struct {
        f32_4x x;
        f32_4x y;
        f32_4x z;
    };
    struct {
        f32_4x r;
        f32_4x g;
        f32_4x b;
    };
    struct {
        v2_4x xy;
        f32_4x reserved1;
    };
    f32_4x e[3];
};

union v4_4x
{
    struct {
        f32_4x x;
        f32_4x y;
        f32_4x z;
        f32_4x w;
    };
    struct {
        f32_4x r;
        f32_4x g;
        f32_4x b;
        f32_4x a;
    };
    struct {
        v2_4x xy;
        v2_4x zw;
    };
    struct {
        v3_4x xyz;
        f32_4x reserved5;
    };
    struct {
        v3_4x rgb;
        f32_4x reserved6;
    };
    f32_4x e[4];
};

struct SinCos_4x
{
    f32_4x sin;
    f32_4x cos;
};

struct SinCos64_2x
{
    f64_2x sin;
    f64_2x cos;
};

internal f32_4x
zero_f32_4x(void)
{
    f32_4x result;
    result.m = _mm_setzero_ps();
    return result;
}

internal f32_4x
F32_4x(f32 f)
{
    f32_4x result;
    result.m = _mm_set1_ps(f);
    return result;
}

internal f32_4x
F32_4x(u32 u)
{
    f32_4x result;
    result.m = _mm_set1_ps(*(f32 *)&u);
    return result;
}

internal f32_4x
F32_4x(f32 f0, f32 f1, f32 f2, f32 f3)
{
    f32_4x result;
    result.m = _mm_setr_ps(f0, f1, f2, f3);
    return result;
}

internal f32_4x
F32_4x(f32 *f)
{
    f32_4x result;
    result.m = _mm_load_ps(f);
    return result;
}

internal f32_4x
S32_4x(s32 s)
{
    f32_4x result;
    result.m = _mm_set1_epi32(s);
    return result;
}

internal f32_4x
S32_4x(s32 s0, s32 s1, s32 s2, s32 s3)
{
    f32_4x result;
    result.m = _mm_setr_epi32(s0, s1, s2, s3);
    return result;
}

internal f32_4x
operator -(f32_4x f4)
{
    u32 signMask = (u32)(1 << 31);
    __m128 flipMask = _mm_set1_ps(*(f32 *)&signMask);
    f32_4x result;
    result.m = _mm_xor_ps(f4.m, flipMask);
    return result;
}

internal f32_4x
operator +(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_add_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator -(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_sub_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator *(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_mul_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator /(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_div_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator &(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_and_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator |(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_or_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator ^(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_xor_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator ==(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmpeq_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator !=(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmpneq_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator <(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmplt_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator <=(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmple_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator >(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmpgt_ps(a.m, b.m);
    return result;
}

internal f32_4x
operator >=(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.m = _mm_cmpge_ps(a.m, b.m);
    return result;
}

internal f32_4x
and_not(f32_4x a, f32_4x b)
{
    // NOTE(michiel): Returns a & ~b
    f32_4x result;
    result.m = _mm_andnot_ps(b.m, a.m);
    return result;
}

internal f32_4x
square(f32_4x f4)
{
    f32_4x result;
    result = f4 * f4;
    return result;
}

internal f32_4x
square_root(f32_4x f4)
{
    f32_4x result;
    result.m = _mm_sqrt_ps(f4.m);
    return result;
}

internal f32_4x
absolute(f32_4x f4)
{
    u32 maskU32 = ~(u32)(1 << 31);
    __m128 mask = _mm_set1_ps(*(f32 *)&maskU32);

    f32_4x result;
    result.m = _mm_and_ps(f4.m, mask);
    return result;
}

internal f32_4x
floor(f32_4x f4)
{
    f32_4x result;
    result.m = _mm_floor_ps(f4.m);
    return result;
}

internal f32_4x
fraction(f32_4x a)
{
    f32_4x result;
    result = a - floor(a);
    return result;
}

internal f32_4x
modulus(f32_4x a, f32_4x b)
{
    // NOTE(michiel): returns a % b <=> a - floor(a/b) * b
    f32_4x result;
    result = a - (floor(a / b) * b);
    return result;
}

internal f32_4x
lerp(f32_4x min, f32_4x t, f32_4x max)
{
    f32_4x result;
    result = min + ((max - min) * t);
    return result;
}

internal f32_4x
clamp_4x(f32_4x min, f32_4x f, f32_4x max)
{
    f32_4x result;
    result.m = _mm_min_ps(max.m, _mm_max_ps(min.m, f.m));
    return result;
}

internal f32_4x
clamp01_4x(f32_4x f)
{
    return clamp_4x(zero_f32_4x(), f, F32_4x(1.0f));
}

internal b32
any(f32_4x f4)
{
    b32 result;
    result = f4.u[0] || f4.u[1] || f4.u[2] || f4.u[3];
    return result;
}

internal b32
all(f32_4x f4)
{
    b32 result;
    result = f4.u[0] && f4.u[1] && f4.u[2] && f4.u[3];
    return result;
}

internal f32_4x
select(f32_4x op0, f32_4x mask, f32_4x op1)
{
    f32_4x result;
    result.m = _mm_or_ps(_mm_andnot_ps(mask.m, op0.m),
                         _mm_and_ps(mask.m, op1.m));
    return result;
}

//
// TODO(michiel): How to handle this s32_4x stuff
//

internal f32_4x
s32_4x_from_f32(f32_4x a)
{
    f32_4x result;
    result.mi = _mm_cvtps_epi32(a.m);
    return result;
}

internal f32_4x
f32_4x_from_s32(f32_4x a)
{
    f32_4x result;
    result.m = _mm_cvtepi32_ps(a.mi);
    return result;
}

internal f32_4x
s32_4x_and(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_and_si128(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_or(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_or_si128(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_add(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_add_epi32(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_sub(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_sub_epi32(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_sll(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_sll_epi32(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_srl(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_srl_epi32(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_sra(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_sra_epi32(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_equal(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_cmpeq_epi32(a.mi, b.mi);
    return result;
}

internal f32_4x
s32_4x_greater(f32_4x a, f32_4x b)
{
    f32_4x result;
    result.mi = _mm_cmpgt_epi32(a.mi, b.mi);
    return result;
}

//
// End of s32_4x stuff
//

//
//
// NOTE(michiel): Vectors
//
//

//
// NOTE(michiel): V2
//

internal v2_4x
zero_v2_4x(void)
{
    v2_4x result;
    result.x.m = _mm_setzero_ps();
    result.y.m = _mm_setzero_ps();
    return result;
}

internal v2_4x
V2_4x(f32 x, f32 y)
{
    v2_4x result;
    result.x.m = _mm_set1_ps(x);
    result.y.m = _mm_set1_ps(y);
    return result;
}

internal v2_4x
V2_4x(v2 vec0, v2 vec1, v2 vec2, v2 vec3)
{
    v2_4x result;
    result.x.m = _mm_setr_ps(vec0.x, vec1.x, vec2.x, vec3.x);
    result.y.m = _mm_setr_ps(vec0.y, vec1.y, vec2.y, vec3.y);
    return result;
}

internal v2_4x
V2_4x(f32 *f, b32 interleaved = true)
{
    v2_4x result;
    result.x.m = _mm_load_ps(f);
    result.y.m = _mm_load_ps(f + 4);
    if (interleaved)
    {
        f32_4x newX;
        // TODO(michiel): Use unpackhi/lo
        newX.m = _mm_shuffle_ps(result.x.m, result.y.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
        result.y.m = _mm_shuffle_ps(result.x.m, result.y.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));
        result.x = newX;
    }
    return result;
}

internal v2
get_elem(v2_4x vec, u32 index)
{
    v2 result;
    result.x = vec.x.e[index];
    result.y = vec.y.e[index];
    return result;
}

internal v2_4x
operator -(v2_4x vec)
{
    v2_4x result;
    result.x = -vec.x;
    result.y = -vec.y;
    return result;
}

internal v2_4x
operator +(v2_4x a, v2_4x b)
{
    v2_4x result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

internal v2_4x
operator -(v2_4x a, v2_4x b)
{
    v2_4x result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

internal v2_4x
operator *(v2_4x a, f32_4x b)
{
    v2_4x result;
    result.x = a.x * b;
    result.y = a.y * b;
    return result;
}

internal v2_4x
operator *(f32_4x a, v2_4x b)
{
    return b * a;
}

internal v2_4x
hadamard(v2_4x a, v2_4x b)
{
    v2_4x result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

internal v2_4x
operator /(v2_4x a, f32_4x b)
{
    v2_4x result;
    result.x = a.x / b;
    result.y = a.y / b;
    return result;
}

internal v2_4x
operator /(f32_4x a, v2_4x b)
{
    v2_4x result;
    result.x = a / b.x;
    result.y = a / b.y;
    return result;
}

internal f32_4x
operator ==(v2_4x a, v2_4x b)
{
    f32_4x result;
    result = (a.x == b.x) & (a.y == b.y);
    return result;
}

internal f32_4x
operator !=(v2_4x a, v2_4x b)
{
    f32_4x result;
    result = (a.x != b.x) | (a.y != b.y);
    return result;
}

internal v2_4x
lerp(v2_4x min, f32_4x t, v2_4x max)
{
    v2_4x result;
    result.x = min.x + ((max.x - min.x) * t);
    result.y = min.y + ((max.y - min.y) * t);
    return result;
}

internal v2_4x
clamp_4x(f32_4x min, v2_4x f, f32_4x max)
{
    v2_4x result;
    result.x = clamp_4x(min, f.x, max);
    result.y = clamp_4x(min, f.y, max);
    return result;
}

internal v2_4x
clamp_4x(v2_4x min, v2_4x f, v2_4x max)
{
    v2_4x result;
    result.x = clamp_4x(min.x, f.x, max.x);
    result.y = clamp_4x(min.y, f.y, max.y);
    return result;
}

internal v2_4x
clamp01_4x(v2_4x f)
{
    v2_4x result;
    f32_4x zero = zero_f32_4x();
    f32_4x one  = F32_4x(1.0f);
    result.x = clamp_4x(zero, f.x, one);
    result.y = clamp_4x(zero, f.y, one);
    return result;
}

internal v2_4x
select(v2_4x op0, f32_4x mask, v2_4x op1)
{
    v2_4x result;
    result.x = select(op0.x, mask, op1.x);
    result.y = select(op0.y, mask, op1.y);
    return result;
}

//
// NOTE(michiel): V3
//

internal v3_4x
zero_v3_4x(void)
{
    v3_4x result;
    result.x.m = _mm_setzero_ps();
    result.y.m = _mm_setzero_ps();
    result.z.m = _mm_setzero_ps();
    return result;
}

internal v3_4x
V3_4x(f32 x, f32 y, f32 z)
{
    v3_4x result;
    result.x.m = _mm_set1_ps(x);
    result.y.m = _mm_set1_ps(y);
    result.z.m = _mm_set1_ps(z);
    return result;
}

internal v3_4x
V3_4x(v3 vec)
{
    return V3_4x(vec.x, vec.y, vec.z);
}

internal v3_4x
V3_4x(v3 vec0, v3 vec1, v3 vec2, v3 vec3)
{
    v3_4x result;
    result.x.m = _mm_setr_ps(vec0.x, vec1.x, vec2.x, vec3.x);
    result.y.m = _mm_setr_ps(vec0.y, vec1.y, vec2.y, vec3.y);
    result.z.m = _mm_setr_ps(vec0.z, vec1.z, vec2.z, vec3.z);
    return result;
}

internal v3_4x
V3_4x(f32 *f, b32 interleaved = true)
{
    v3_4x result;
    result.x.m = _mm_load_ps(f);
    result.y.m = _mm_load_ps(f + 4);
    result.z.m = _mm_load_ps(f + 8);
    if (interleaved)
    {
        f32_4x temp0;
        f32_4x temp1;
        temp0.m = _mm_shuffle_ps(result.x.m, result.y.m, MULTILANE_SHUFFLE_MASK(1, 2, 0, 1));
        temp1.m = _mm_shuffle_ps(result.y.m, result.z.m, MULTILANE_SHUFFLE_MASK(2, 3, 1, 2));
        result.x.m = _mm_shuffle_ps(result.x.m, temp1.m, MULTILANE_SHUFFLE_MASK(0, 3, 0, 2));
        result.y.m = _mm_shuffle_ps(temp0.m, temp1.m, MULTILANE_SHUFFLE_MASK(0, 2, 1, 3));
        result.z.m = _mm_shuffle_ps(temp0.m, result.z.m, MULTILANE_SHUFFLE_MASK(1, 3, 0, 3));
    }
    return result;
}

internal v3
get_elem(v3_4x vec, u32 index)
{
    v3 result;
    result.x = vec.x.e[index];
    result.y = vec.y.e[index];
    result.z = vec.z.e[index];
    return result;
}

internal v3_4x
operator -(v3_4x vec)
{
    v3_4x result;
    result.x = -vec.x;
    result.y = -vec.y;
    result.z = -vec.z;
    return result;
}

internal v3_4x
operator +(v3_4x a, v3_4x b)
{
    v3_4x result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

internal v3_4x
operator -(v3_4x a, v3_4x b)
{
    v3_4x result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

internal v3_4x
operator *(v3_4x a, f32_4x b)
{
    v3_4x result;
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    return result;
}

internal v3_4x
operator *(f32_4x a, v3_4x b)
{
    return b * a;
}

internal v3_4x
hadamard(v3_4x a, v3_4x b)
{
    v3_4x result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

internal v3_4x
operator /(v3_4x a, f32_4x b)
{
    v3_4x result;
    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;
    return result;
}

internal v3_4x
operator /(f32_4x a, v3_4x b)
{
    v3_4x result;
    result.x = a / b.x;
    result.y = a / b.y;
    result.z = a / b.z;
    return result;
}

internal f32_4x
operator ==(v3_4x a, v3_4x b)
{
    f32_4x result;
    result = (a.x == b.x) & (a.y == b.y) & (a.z == b.z);
    return result;
}

internal f32_4x
operator !=(v3_4x a, v3_4x b)
{
    f32_4x result;
    result = (a.x != b.x) | (a.y != b.y) | (a.z != b.z);
    return result;
}

internal v3_4x
lerp(v3_4x min, f32_4x t, v3_4x max)
{
    v3_4x result;
    result.x = min.x + ((max.x - min.x) * t);
    result.y = min.y + ((max.y - min.y) * t);
    result.z = min.z + ((max.z - min.z) * t);
    return result;
}

internal v3_4x
clamp_4x(f32_4x min, v3_4x f, f32_4x max)
{
    v3_4x result;
    result.x = clamp_4x(min, f.x, max);
    result.y = clamp_4x(min, f.y, max);
    result.z = clamp_4x(min, f.z, max);
    return result;
}

internal v3_4x
clamp_4x(v3_4x min, v3_4x f, v3_4x max)
{
    v3_4x result;
    result.x = clamp_4x(min.x, f.x, max.x);
    result.y = clamp_4x(min.y, f.y, max.y);
    result.z = clamp_4x(min.z, f.z, max.z);
    return result;
}

internal v3_4x
clamp01_4x(v3_4x f)
{
    v3_4x result;
    f32_4x zero = zero_f32_4x();
    f32_4x one  = F32_4x(1.0f);
    result.x = clamp_4x(zero, f.x, one);
    result.y = clamp_4x(zero, f.y, one);
    result.z = clamp_4x(zero, f.z, one);
    return result;
}

internal v3_4x
select(v3_4x op0, f32_4x mask, v3_4x op1)
{
    v3_4x result;
    result.x = select(op0.x, mask, op1.x);
    result.y = select(op0.y, mask, op1.y);
    result.z = select(op0.z, mask, op1.z);
    return result;
}

//
// NOTE(michiel): V4
//

internal v4_4x
zero_v4_4x(void)
{
    v4_4x result;
    result.x.m = _mm_setzero_ps();
    result.y.m = _mm_setzero_ps();
    result.z.m = _mm_setzero_ps();
    result.w.m = _mm_setzero_ps();
    return result;
}

internal v4_4x
V4_4x(f32 x, f32 y, f32 z, f32 w)
{
    v4_4x result;
    result.x.m = _mm_set1_ps(x);
    result.y.m = _mm_set1_ps(y);
    result.z.m = _mm_set1_ps(z);
    result.w.m = _mm_set1_ps(w);
    return result;
}

internal v4_4x
V4_4x(v4 vec0, v4 vec1, v4 vec2, v4 vec3)
{
    v4_4x result;
    result.x.m = _mm_setr_ps(vec0.x, vec1.x, vec2.x, vec3.x);
    result.y.m = _mm_setr_ps(vec0.y, vec1.y, vec2.y, vec3.y);
    result.z.m = _mm_setr_ps(vec0.z, vec1.z, vec2.z, vec3.z);
    result.w.m = _mm_setr_ps(vec0.w, vec1.w, vec2.w, vec3.w);
    return result;
}

internal v4_4x
V4_4x(f32 *f, b32 interleaved = true)
{
    v4_4x result;
    result.x.m = _mm_load_ps(f);
    result.y.m = _mm_load_ps(f + 4);
    result.z.m = _mm_load_ps(f + 8);
    result.w.m = _mm_load_ps(f + 12);
    if (interleaved)
    {
        f32_4x temp0;
        f32_4x temp1;
        f32_4x temp2;
        f32_4x temp3;
        temp0.m = _mm_shuffle_ps(result.x.m, result.y.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
        temp1.m = _mm_shuffle_ps(result.x.m, result.y.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
        temp2.m = _mm_shuffle_ps(result.z.m, result.w.m, MULTILANE_SHUFFLE_MASK(0, 1, 0, 1));
        temp3.m = _mm_shuffle_ps(result.z.m, result.w.m, MULTILANE_SHUFFLE_MASK(2, 3, 2, 3));
        result.x.m = _mm_shuffle_ps(temp0.m, temp2.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
        result.y.m = _mm_shuffle_ps(temp0.m, temp2.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));
        result.z.m = _mm_shuffle_ps(temp1.m, temp3.m, MULTILANE_SHUFFLE_MASK(0, 2, 0, 2));
        result.w.m = _mm_shuffle_ps(temp1.m, temp3.m, MULTILANE_SHUFFLE_MASK(1, 3, 1, 3));
    }
    return result;
}

internal v4
get_elem(v4_4x vec, u32 index)
{
    v4 result;
    result.x = vec.x.e[index];
    result.y = vec.y.e[index];
    result.z = vec.z.e[index];
    result.w = vec.w.e[index];
    return result;
}

internal v4_4x
operator -(v4_4x vec)
{
    v4_4x result;
    result.x = -vec.x;
    result.y = -vec.y;
    result.z = -vec.z;
    result.w = -vec.w;
    return result;
}

internal v4_4x
operator +(v4_4x a, v4_4x b)
{
    v4_4x result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    return result;
}

internal v4_4x
operator -(v4_4x a, v4_4x b)
{
    v4_4x result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;
    return result;
}

internal v4_4x
operator *(v4_4x a, f32_4x b)
{
    v4_4x result;
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    result.w = a.w * b;
    return result;
}

internal v4_4x
operator *(f32_4x a, v4_4x b)
{
    return b * a;
}

internal v4_4x
hadamard(v4_4x a, v4_4x b)
{
    v4_4x result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    result.w = a.w * b.w;
    return result;
}

internal v4_4x
operator /(v4_4x a, f32_4x b)
{
    v4_4x result;
    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;
    result.w = a.w / b;
    return result;
}

internal v4_4x
operator /(f32_4x a, v4_4x b)
{
    v4_4x result;
    result.x = a / b.x;
    result.y = a / b.y;
    result.z = a / b.z;
    result.w = a / b.w;
    return result;
}

internal f32_4x
operator ==(v4_4x a, v4_4x b)
{
    f32_4x result;
    result = (a.x == b.x) & (a.y == b.y) & (a.z == b.z) & (a.w == b.w);
    return result;
}

internal f32_4x
operator !=(v4_4x a, v4_4x b)
{
    f32_4x result;
    result = (a.x != b.x) | (a.y != b.y) | (a.z != b.z) | (a.w != b.w);
    return result;
}

internal v4_4x
lerp(v4_4x min, f32_4x t, v4_4x max)
{
    v4_4x result;
    result.x = min.x + ((max.x - min.x) * t);
    result.y = min.y + ((max.y - min.y) * t);
    result.z = min.z + ((max.z - min.z) * t);
    result.w = min.w + ((max.w - min.w) * t);
    return result;
}

internal v4_4x
clamp_4x(f32_4x min, v4_4x f, f32_4x max)
{
    v4_4x result;
    result.x = clamp_4x(min, f.x, max);
    result.y = clamp_4x(min, f.y, max);
    result.z = clamp_4x(min, f.z, max);
    result.w = clamp_4x(min, f.w, max);
    return result;
}

internal v4_4x
clamp_4x(v4_4x min, v4_4x f, v4_4x max)
{
    v4_4x result;
    result.x = clamp_4x(min.x, f.x, max.x);
    result.y = clamp_4x(min.y, f.y, max.y);
    result.z = clamp_4x(min.z, f.z, max.z);
    result.w = clamp_4x(min.w, f.w, max.w);
    return result;
}

internal v4_4x
clamp01_4x(v4_4x f)
{
    v4_4x result;
    f32_4x zero = zero_f32_4x();
    f32_4x one  = F32_4x(1.0f);
    result.x = clamp_4x(zero, f.x, one);
    result.y = clamp_4x(zero, f.y, one);
    result.z = clamp_4x(zero, f.z, one);
    result.w = clamp_4x(zero, f.w, one);
    return result;
}

internal v4_4x
select(v4_4x op0, f32_4x mask, v4_4x op1)
{
    v4_4x result;
    result.x = select(op0.x, mask, op1.x);
    result.y = select(op0.y, mask, op1.y);
    result.z = select(op0.z, mask, op1.z);
    result.w = select(op0.w, mask, op1.w);
    return result;
}

internal SinCos_4x
sincos_f32_4x_approx_small(f32_4x angles)
{
    // NOTE(michiel): Valid for angles between -0.5 and 0.5
    SinCos_4x result;

    f32_4x one_4x = F32_4x(1.0f);
    f32_4x halfPi_4x = F32_4x(0.5f * F32_PI);

    // cos(x) = 1 - (x^2/2!) + (x^4/4!) - (x^6/6!) + (x^8/8!)
    // cosA(x) = x^2/2! + x^6/6! = x^2 * (1/2! + x^4/6!)
    // cosB(x) = x^4/4! + x^8/8! = x^4 * (1/4! + x^4/8!)
    // cos(x) = 1 - cosA(x) + cosB(x)
    f32_4x c0 = F32_4x(1.2337005501361697490381175157381221652030944824f); // NOTE(michiel): (pi^2 /   4) / 2!
    f32_4x c1 = F32_4x(0.2536695079010479747516626503056613728404045105f); // NOTE(michiel): (pi^4 /  16) / 4!
    f32_4x c2 = F32_4x(0.0208634807633529574533159944849103339947760105f); // NOTE(michiel): (pi^6 /  64) / 6!
    f32_4x c3 = F32_4x(0.0009192602748394262598963244670358108123764396f); // NOTE(michiel): (pi^8 / 256) / 8!

    // sin(x) = x - (x^3/3!) + (x^5/5!) - (x^7/7!) + (x^9/9!)
    // sin(x) = x * (1 - (x^2/3!) + (x^4/5!) - (x^6/7!) + (x^8/9!)
    // sinA(x) = x^2/3! + x^6/7! = x^2 * (1/3! + x^4/7!)
    // sinB(x) = x^4/5! + x^8/9! = x^4 * (1/5! + x^4/9!)
    // sin(x) = x * (1 - sinA(x) + sinB(x))
    f32_4x s0 = F32_4x(0.4112335167120566015164229156653163954615592956f); // NOTE(michiel): (pi^2 /   4) / 3!
    f32_4x s1 = F32_4x(0.0507339015802095935625537492796865990385413170f); // NOTE(michiel): (pi^4 /  16) / 5!
    f32_4x s2 = F32_4x(0.0029804972519075654743825332104734116001054644f); // NOTE(michiel): (pi^6 /  64) / 7!
    f32_4x s3 = F32_4x(0.0001021400305377140213481876318546426318789599f); // NOTE(michiel): (pi^8 / 256) / 9!

    f32_4x angSq = square(angles);
    f32_4x angQd = square(square(angles));

    f32_4x cosA = angSq * (c0 + angQd * c2);
    f32_4x cosB = angQd * (c1 + angQd * c3);
    f32_4x sinA = angSq * (s0 + angQd * s2);
    f32_4x sinB = angQd * (s1 + angQd * s3);

    result.cos = one_4x - cosA + cosB;
    result.sin = halfPi_4x * angles * (one_4x - sinA + sinB);

    return result;
}

#if 1
internal f32_4x
cos_f32_4x(f32_4x angles)
{
    f32_4x eight_4x = F32_4x(0.125f);
    f32_4x one_4x   = F32_4x(1.0f);

    angles = angles + eight_4x;
    //angles = modulus(angles, one_4x);
    angles = fraction(angles);
    angles = angles - eight_4x;
    angles = absolute(angles * F32_4x(4.0f));

    f32_4x quadrant4 = angles > F32_4x(2.5f);
    f32_4x quadrant3 = angles > F32_4x(1.5f);
    f32_4x quadrant2 = angles > F32_4x(0.5f);

    quadrant2 = and_not(quadrant2, quadrant3);
    quadrant3 = and_not(quadrant3, quadrant4);

    f32_4x quadrants = (F32_4x(3.0f) & quadrant4) | (F32_4x(2.0f) & quadrant3) | (one_4x & quadrant2);
    angles = angles - quadrants;

    f32_4x do_cos_mask = quadrant3 | (quadrants == zero_f32_4x());
    f32_4x do_inv_mask = quadrant2 | quadrant3;

    SinCos_4x sincos = sincos_f32_4x_approx_small(angles);
    f32_4x result = select(sincos.sin, do_cos_mask, sincos.cos);
    result = select(result, do_inv_mask, -result);

    return result;
}

internal f32_4x
sin_f32_4x(f32_4x angles)
{
    f32_4x quarter_4x = F32_4x(0.25f);
    f32_4x result = cos_f32_4x(angles - quarter_4x);
    return result;
}

internal SinCos_4x
sincos_4x(f32_4x angles)
{
    // NOTE(michiel): Getting the sine and cosine of some angle is basically free.
    f32_4x eight_4x = F32_4x(0.125f);
    f32_4x one_4x   = F32_4x(1.0f);

    angles = angles + eight_4x;
    angles = fraction(angles);
    angles = angles - eight_4x;
    angles = angles * F32_4x(4.0f);

    f32_4x quadrant4 = angles > F32_4x(2.5f);
    f32_4x quadrant3 = angles > F32_4x(1.5f);
    f32_4x quadrant2 = angles > F32_4x(0.5f);

    quadrant2 = and_not(quadrant2, quadrant3);
    quadrant3 = and_not(quadrant3, quadrant4);

    f32_4x quadrants = (F32_4x(3.0f) & quadrant4) | (F32_4x(2.0f) & quadrant3) | (one_4x & quadrant2);
    angles = angles - quadrants;

    f32_4x do_cos_mask = quadrant3 | (quadrants == zero_f32_4x());
    f32_4x do_invc_mask = quadrant2 | quadrant3;
    f32_4x do_invs_mask = quadrant3 | quadrant4;

    SinCos_4x sincos = sincos_f32_4x_approx_small(angles);
    SinCos_4x result;
    result.cos = select(sincos.sin, do_cos_mask, sincos.cos);
    result.cos = select(result.cos, do_invc_mask, -result.cos);
    result.sin = select(sincos.cos, do_cos_mask, sincos.sin);
    result.sin = select(result.sin, do_invs_mask, -result.sin);

    return result;
}

#else
internal f32_4x
cos_f32_4x(f32_4x angles)
{
    f32_4x result;
    result.e[0] = __builtin_cosf(F32_TAU * angles.e[0]);
    result.e[1] = __builtin_cosf(F32_TAU * angles.e[1]);
    result.e[2] = __builtin_cosf(F32_TAU * angles.e[2]);
    result.e[3] = __builtin_cosf(F32_TAU * angles.e[3]);
    return result;
}

internal f32_4x
sin_f32_4x(f32_4x angles)
{
    f32_4x result;
    result.e[0] = __builtin_sinf(F32_TAU * angles.e[0]);
    result.e[1] = __builtin_sinf(F32_TAU * angles.e[1]);
    result.e[2] = __builtin_sinf(F32_TAU * angles.e[2]);
    result.e[3] = __builtin_sinf(F32_TAU * angles.e[3]);
    return result;
}
#endif

internal f32_4x
tan_f32_4x(f32_4x angles)
{
    f32_4x one_4x   = F32_4x(1.0f);
    angles = angles * F32_4x(2.0f);

#if 0
    // TODO(michiel): With sincos_f32_4x_approx_small
    f32_4x eight_4x = F32_4x(0.125f);

    angles = angles + eight_4x;
    angles = modulus(angles, one_4x);
    angles = angles - eight_4x;
    angles = absolute(angles * F32_4x(4.0f));

    f32_4x quadrant4 = angles > F32_4x(2.5f);
    f32_4x quadrant3 = angles > F32_4x(1.5f);
    f32_4x quadrant2 = angles > F32_4x(0.5f);

    quadrant2 = and_not(quadrant2, quadrant3);
    quadrant3 = and_not(quadrant3, quadrant4);

    f32_4x quadrants = (F32_4x(3.0f) & quadrant4) | (F32_4x(2.0f) & quadrant3) | (one_4x & quadrant2);
    angles = angles - quadrants;

    SinCos_4x sincos = sincos_f32_4x_approx_small(angles);
    f32_4x do_cos_mask = quadrant3 | (quadrants == zero_f32_4x());
    f32_4x do_inv_cos_mask = quadrant2 | quadrant3;
    f32_4x do_inv_sin_mask = quadrant3 | quadrant4;

    f32_4x cosine = select(sincos.sin, do_cos_mask, sincos.cos);
    cosine = select(cosine, do_inv_cos_mask, -cosine);
    f32_4x sine = select(sincos.cos, do_cos_mask, sincos.sin);
    sine = select(sine, do_inv_sin_mask, -sine);
#else
    f32_4x cosine = cos_f32_4x(angles);
    f32_4x sine = sin_f32_4x(angles);
#endif

    f32_4x result = sine / (one_4x + cosine);
    return result;
}

internal f32_4x
exp_f32_4x(f32_4x f4)
{
    // NOTE(michiel): Change the 12 (and the 12 multiplies) to tune the approximation
    f32_4x result = F32_4x(1.0f);
    f32_4x divisor = F32_4x(1.0f / (f32)(1 << 12));
    result = result + f4 * divisor;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    result = result * result;
    return result;
}
