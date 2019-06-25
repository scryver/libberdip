#define PERLIN_DEPTH 256
#define PERLIN_DEPTH_MASK 0xFF

#define PERLIN_N     0x1000

struct PerlinNoise
{
    u8 permutations[PERLIN_DEPTH];
};

internal void
init_perlin_noise(PerlinNoise *perlin, RandomSeriesPCG *random)
{
    for (u32 i = 0; i < PERLIN_DEPTH; ++i)
    {
        perlin->permutations[i] = i;
    }

    for (u32 i = 0; i < PERLIN_DEPTH; ++i)
    {
        u32 index = random_choice(random, PERLIN_DEPTH);
        u8 temp = perlin->permutations[index];
        perlin->permutations[index] = perlin->permutations[i];
        perlin->permutations[i] = temp;
    }
}

#define s_curve(t) (t * t * t * (10.0f + (6.0f * t - 15.0f) * t))

internal inline f32
gradient(u32 hash, f32 x, f32 y, f32 z)
{
    u32 h = hash & 0xF;
    f32 u = (h < 8) ? x : y;
    f32 v = (h < 4) ? y : (((h == 12) || (h == 14)) ? x : z);
    return (((h & 1) == 0) ? u : -u) + (((h & 2) == 0) ? v : -v);
}

internal f32
perlin_noise(PerlinNoise *perlin, v3 p)
{
    p += PERLIN_N;
    v3u b0 = V3U(u32_from_f32_truncate(p.x),
                 u32_from_f32_truncate(p.y),
                 u32_from_f32_truncate(p.z));
    b0 &= PERLIN_DEPTH_MASK;

    v3 r0 = p;
    r0.x -= (f32)s32_from_f32_truncate(r0.x);
    r0.y -= (f32)s32_from_f32_truncate(r0.y);
    r0.z -= (f32)s32_from_f32_truncate(r0.z);
    v3 r1 = r0 - 1.0f;

    f32 sx = s_curve(r0.x);
    f32 sy = s_curve(r0.y);
    f32 sz = s_curve(r0.z);

    u32 i = perlin->permutations[b0.x] + b0.y;
    u32 j = perlin->permutations[b0.x + 1] + b0.y;
    u32 ii = perlin->permutations[i & PERLIN_DEPTH_MASK] + b0.z;
    u32 ji = perlin->permutations[j & PERLIN_DEPTH_MASK] + b0.z;
    u32 ij = perlin->permutations[(i + 1) & PERLIN_DEPTH_MASK] + b0.z;
    u32 jj = perlin->permutations[(j + 1) & PERLIN_DEPTH_MASK] + b0.z;

    f32 u, v, a, b, c, d;

    u = gradient(perlin->permutations[ii & PERLIN_DEPTH_MASK], r0.x, r0.y, r0.z);
    v = gradient(perlin->permutations[ji & PERLIN_DEPTH_MASK], r1.x, r0.y, r0.z);
    a = lerp(u, sx, v);

    u = gradient(perlin->permutations[ij & PERLIN_DEPTH_MASK], r0.x, r1.y, r0.z);
    v = gradient(perlin->permutations[jj & PERLIN_DEPTH_MASK], r1.x, r1.y, r0.z);
    b = lerp(u, sx, v);

    c = lerp(a, sy, b);

    u = gradient(perlin->permutations[(ii + 1) & PERLIN_DEPTH_MASK], r0.x, r0.y, r1.z);
    v = gradient(perlin->permutations[(ji + 1) & PERLIN_DEPTH_MASK], r1.x, r0.y, r1.z);
    a = lerp(u, sx, v);

    u = gradient(perlin->permutations[(ij + 1) & PERLIN_DEPTH_MASK], r0.x, r1.y, r1.z);
    v = gradient(perlin->permutations[(jj + 1) & PERLIN_DEPTH_MASK], r1.x, r1.y, r1.z);
    b = lerp(u, sx, v);

    d = lerp(a, sy, b);

    return lerp(c, sz, d);
}

internal inline f32
perlin_noise(PerlinNoise *perlin, v2 p)
{
    p += PERLIN_N;
    v2u b0 = V2U(u32_from_f32_truncate(p.x), u32_from_f32_truncate(p.y));
    b0 &= PERLIN_DEPTH_MASK;

    v2 r0 = p;
    r0.x -= (f32)s32_from_f32_truncate(r0.x);
    r0.y -= (f32)s32_from_f32_truncate(r0.y);
    v2 r1 = r0 - 1.0f;

    f32 sx = s_curve(r0.x);
    f32 sy = s_curve(r0.y);

    u32 i = perlin->permutations[b0.x] + b0.y;
    u32 j = perlin->permutations[b0.x + 1] + b0.y;
    u32 ii = perlin->permutations[i & PERLIN_DEPTH_MASK];
    u32 ji = perlin->permutations[j & PERLIN_DEPTH_MASK];
    u32 ij = perlin->permutations[(i + 1) & PERLIN_DEPTH_MASK];
    u32 jj = perlin->permutations[(j + 1) & PERLIN_DEPTH_MASK];

    f32 u, v, a, b;

    u = gradient(perlin->permutations[ii & PERLIN_DEPTH_MASK], r0.x, r0.y, 0);
    v = gradient(perlin->permutations[ji & PERLIN_DEPTH_MASK], r1.x, r0.y, 0);
    a = lerp(u, sx, v);

    u = gradient(perlin->permutations[ij & PERLIN_DEPTH_MASK], r0.x, r1.y, 0);
    v = gradient(perlin->permutations[jj & PERLIN_DEPTH_MASK], r1.x, r1.y, 0);
    b = lerp(u, sx, v);

    return lerp(a, sy, b);
}

internal inline f32
perlin_noise(PerlinNoise *perlin, f32 p)
{
    p += PERLIN_N;
    u32 b0 = u32_from_f32_truncate(p);
    b0 &= PERLIN_DEPTH_MASK;
    u32 b1 = b0 + 1;
    b1 &= PERLIN_DEPTH_MASK;

    f32 r0 = p;
    r0 -= s32_from_f32_truncate(r0);
    f32 r1 = r0 - 1.0f;

    f32 sx = s_curve(r0);

    u32 i = perlin->permutations[b0] & PERLIN_DEPTH_MASK;
    u32 j = perlin->permutations[b1] & PERLIN_DEPTH_MASK;
    u32 ii = perlin->permutations[i] & PERLIN_DEPTH_MASK;
    u32 ji = perlin->permutations[j] & PERLIN_DEPTH_MASK;

    f32 u, v;

    u = gradient(perlin->permutations[ii], r0, r1, 0);
    v = gradient(perlin->permutations[ji], r1, r0, 0);

    return lerp(u, sx, v);
}

#undef s_curve

#if 1
// NOTE(michiel): Original perlin code below

#define PERLIN_NP    12  // 2 ^ NP == N
#define PERLIN_NMASK 0xFFF

struct PerlinNoiseOld
{
    u8 permutations[PERLIN_DEPTH + PERLIN_DEPTH + 2];
    v3 gradients3[PERLIN_DEPTH + PERLIN_DEPTH + 2];
    v2 gradients2[PERLIN_DEPTH + PERLIN_DEPTH + 2];
    f32 gradients[PERLIN_DEPTH + PERLIN_DEPTH + 2];
};

internal void
init_perlin_noise(PerlinNoiseOld *noise, RandomSeriesPCG *random)
{
    s32 i;
    for (i = 0; i < PERLIN_DEPTH; ++i)
    {
        noise->permutations[i] = i;

        noise->gradients[i] = random_bilateral(random);

        v2 grad2 = V2(random_bilateral(random), random_bilateral(random));
        noise->gradients2[i] = normalize(grad2);

        v3 grad3 = V3(random_bilateral(random), random_bilateral(random),
                      random_bilateral(random));
        noise->gradients3[i] = normalize_or_zero(grad3);
    }

    while (--i)
    {
        u32 randJ = random_choice(random, PERLIN_DEPTH);
        u8 temp = noise->permutations[i];
        noise->permutations[i] = noise->permutations[randJ];
        noise->permutations[randJ] = temp;
    }

    for (i = 0; i < PERLIN_DEPTH + 2; ++i)
    {
        noise->permutations[PERLIN_DEPTH + i] = noise->permutations[i];
        noise->gradients[PERLIN_DEPTH + i] = noise->gradients[i];
        noise->gradients2[PERLIN_DEPTH + i] = noise->gradients2[i];
        noise->gradients3[PERLIN_DEPTH + i] = noise->gradients3[i];
    }
}

#define s_curve(t) (t * t * (3.0f - 2.0f * t))
#define setup(i, t, b0, b1, r0, r1) \
t = vec[i] + PERLIN_N; \
b0 = s32_from_f32_truncate(t) & PERLIN_DEPTH_MASK; \
b1 = (b0 + 1) & PERLIN_DEPTH_MASK; \
r0 = t - (f32)s32_from_f32_truncate(t); \
r1 = r0 - 1.0f

internal inline f32
perlin_noise(PerlinNoiseOld *perlin, f32 p)
{
    s32 bx0, bx1;
    f32 rx0, rx1, g, vec[1];

    vec[0] = p;
    setup(0, g, bx0, bx1, rx0, rx1);

    f32 sx = s_curve(rx0);

    f32 u = rx0 * perlin->gradients[perlin->permutations[bx0]];
    f32 v = rx1 * perlin->gradients[perlin->permutations[bx1]];

    return lerp(u, sx, v);
}

internal inline f32
perlin_noise(PerlinNoiseOld *perlin, v2 p)
{
#if 0
    s32 bx0, bx1, by0, by1;
    f32 rx0, rx1, ry0, ry1, g, vec[2];

    vec[0] = p.x;
    vec[1] = p.y;

    setup(0, g, bx0, bx1, rx0, rx1);
    setup(1, g, by0, by1, ry0, ry1);

    u8 i = perlin->permutations[bx0];
    u8 j = perlin->permutations[bx1];

    s32 b00 = perlin->permutations[i + by0];
    s32 b10 = perlin->permutations[j + by0];
    s32 b01 = perlin->permutations[i + by1];
    s32 b11 = perlin->permutations[j + by1];

    f32 sx = s_curve(rx0);
    f32 sy = s_curve(ry0);

    v2 q = perlin->gradients2[b00];
    f32 u = rx0 * q.x + ry0 * q.y;
    q = perlin->gradients2[b10];
    f32 v = rx1 * q.x + ry0 * q.y;
    f32 a = lerp(u, sx, v);

    q = perlin->gradients2[b01];
    u = rx0 * q.x + ry1 * q.y;
    q = perlin->gradients2[b11];
    v = rx1 * q.x + ry1 * q.y;
    f32 b = lerp(u, sx, v);

    return lerp(a, sy, b);
#else
    v2 offsetter = p + PERLIN_N;
    v2u b0 = V2U(u32_from_f32_truncate(offsetter.x), u32_from_f32_truncate(offsetter.y));
    b0.x = u32_from_f32_truncate(b0.x) & PERLIN_DEPTH_MASK;
    b0.y = u32_from_f32_truncate(b0.y) & PERLIN_DEPTH_MASK;
    v2u b1 = b0 + 1;
    b1 &= PERLIN_DEPTH_MASK;

    v2 r0 = p + PERLIN_N;
    r0.x -= s32_from_f32_truncate(r0.x);
    r0.y -= s32_from_f32_truncate(r0.y);
    v2 r1 = r0 - V2(1, 1);

    u8 i = perlin->permutations[b0.x];
    u8 j = perlin->permutations[b1.x];

    s32 b00 = perlin->permutations[i + b0.y];
    s32 b10 = perlin->permutations[j + b0.y];
    s32 b01 = perlin->permutations[i + b1.y];
    s32 b11 = perlin->permutations[j + b1.y];

    f32 sx = s_curve(r0.x);
    f32 sy = s_curve(r0.y);

    f32 u = dot(r0, perlin->gradients2[b00]);
    f32 v = dot(V2(r1.x, r0.y), perlin->gradients2[b10]);
    f32 a = lerp(u, sx, v);

    u = dot(V2(r0.x, r1.y), perlin->gradients2[b01]);
    v = dot(r1, perlin->gradients2[b11]);
    f32 b = lerp(u, sx, v);

    return lerp(a, sy, b);
#endif
}

internal inline f32
perlin_noise(PerlinNoiseOld *perlin, v3 p)
{
#if 1
    s32 bx0, bx1, by0, by1, bz0, bz1;
    f32 rx0, rx1, ry0, ry1, rz0, rz1, g, vec[3];

    vec[0] = p.x;
    vec[1] = p.y;
    vec[2] = p.z;

    setup(0, g, bx0, bx1, rx0, rx1);
    setup(1, g, by0, by1, ry0, ry1);
    setup(2, g, bz0, bz1, rz0, rz1);

    u8 i = perlin->permutations[bx0];
    u8 j = perlin->permutations[bx1];

    s32 b00 = perlin->permutations[i + by0];
    s32 b10 = perlin->permutations[j + by0];
    s32 b01 = perlin->permutations[i + by1];
    s32 b11 = perlin->permutations[j + by1];

    f32 sx = s_curve(rx0);
    f32 sy = s_curve(ry0);
    f32 sz = s_curve(rz0);

    v3 q = perlin->gradients3[b00 + bz0];
    f32 u = rx0 * q.x + ry0 * q.y + rz0 * q.z;
    q = perlin->gradients3[b10 + bz0];
    f32 v = rx1 * q.x + ry0 * q.y + rz0 * q.z;
    f32 a = lerp(u, sx, v);

    q = perlin->gradients3[b01 + bz0];
    u = rx0 * q.x + ry1 * q.y + rz0 * q.z;
    q = perlin->gradients3[b11 + bz0];
    v = rx1 * q.x + ry1 * q.y + rz0 * q.z;
    f32 b = lerp(u, sx, v);

    f32 c = lerp(a, sy, b);

    q = perlin->gradients3[b00 + bz1];
    u = rx0 * q.x + ry0 * q.y + rz1 * q.z;
    q = perlin->gradients3[b10 + bz1];
    v = rx1 * q.x + ry0 * q.y + rz1 * q.z;
    a = lerp(u, sx, v);

    q = perlin->gradients3[b01 + bz1];
    u = rx0 * q.x + ry1 * q.y + rz1 * q.z;
    q = perlin->gradients3[b11 + bz1];
    v = rx1 * q.x + ry1 * q.y + rz1 * q.z;
    b = lerp(u, sx, v);

    f32 d = lerp(a, sy, b);

    return lerp(c, sz, d);
#else
    v3 offsetter = p + PERLIN_N;
    v3u b0 = V3U(u32_from_f32_truncate(offsetter.x), u32_from_f32_truncate(offsetter.y),
                 u32_from_f32_truncate(offsetter.z));
    b0.x = u32_from_f32_truncate(b0.x) & PERLIN_DEPTH_MASK;
    b0.y = u32_from_f32_truncate(b0.y) & PERLIN_DEPTH_MASK;
    b0.z = u32_from_f32_truncate(b0.z) & PERLIN_DEPTH_MASK;
    v3u b1 = b0 + 1;
    b1 &= PERLIN_DEPTH_MASK;

    v3 r0 = p + PERLIN_N;
    r0.x -= trunc(r0.x);
    r0.y -= trunc(r0.y);
    v3 r1 = r0 - 1.0f;

    u8 i = perlin->permutations[b0.x];
    u8 j = perlin->permutations[b1.x];

    s32 b00 = perlin->permutations[i + b0.y];
    s32 b10 = perlin->permutations[j + b0.y];
    s32 b01 = perlin->permutations[i + b1.y];
    s32 b11 = perlin->permutations[j + b1.y];

    f32 sx = s_curve(r0.x);
    f32 sy = s_curve(r0.y);
    f32 sz = s_curve(r0.z);

    f32 u = dot(r0, perlin->gradients3[b00 + b0.z]);
    f32 v = dot(V3(r1.x, r0.y, r0.z), perlin->gradients3[b10 + b0.z]);
    f32 a = lerp(u, sx, v);

    u = dot(V3(r0.x, r1.y, r0.z), perlin->gradients3[b01 + b0.z]);
    v = dot(V3(r1.x, r1.y, r0.z), perlin->gradients3[b11 + b0.z]);
    f32 b = lerp(u, sx, v);

    f32 c = lerp(a, sy, b);

    u = dot(V3(r0.x, r0.y, r1.z), perlin->gradients3[b00 + b1.z]);
    v = dot(V3(r1.x, r0.y, r1.z), perlin->gradients3[b10 + b1.z]);
    a = lerp(u, sx, v);

    u = dot(V3(r0.x, r1.y, r1.z), perlin->gradients3[b01 + b1.z]);
    v = dot(r1, perlin->gradients3[b11 + b1.z]);
    b = lerp(u, sx, v);

    f32 d = lerp(a, sy, b);

    return lerp(c, sz, d);

#endif

}

#undef setup
#undef s_curve

#endif
