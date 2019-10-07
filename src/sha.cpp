// NOTE(michiel): Compile with the -msha -msse4 flags

//
// NOTE(michiel): SHA1
//

internal Sha1Context
sha1_initialize(void)
{
    Sha1Context result = {};
    result.mask = _mm_set_epi64x(0x0001020304050607ULL, 0x08090A0B0C0D0E0FULL);
    result.abcd = _mm_set_epi32(0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476);
    result.e0   = _mm_set_epi32(0xC3D2E1F0, 0, 0, 0);
    return result;
}

internal void
sha1_finish(Sha1Context *context, u32 *output)
{
    // NOTE(michiel): expect output[5]
    context->abcd = _mm_shuffle_epi8(context->abcd, context->mask);
    output[0] = _mm_extract_epi32(context->abcd, 0);
    output[1] = _mm_extract_epi32(context->abcd, 1);
    output[2] = _mm_extract_epi32(context->abcd, 2);
    output[3] = _mm_extract_epi32(context->abcd, 3);
    output[4] = _mm_extract_epi32(context->e0, 3);

    u8 *src = (u8 *)(output + 4);
    output[4] = ((src[0] << 24) | (src[1] << 16) | (src[2] <<  8) | (src[3] <<  0));
}

internal void
sha1_process_chunk(Sha1Context *context, u8 *chunk)
{
    // NOTE(michiel): https://software.intel.com/en-us/articles/intel-sha-extensions
    // NOTE(michiel): expect chunk[64]
    __m128i *src = (__m128i *)chunk;
    __m128i e1;

    __m128i origAbcd = context->abcd;
    __m128i origE0   = context->e0;

    // NOTE(michiel): 0 - 4 / 80
    __m128i src0_4x = _mm_loadu_si128(src + 0);
    src0_4x = _mm_shuffle_epi8(src0_4x, context->mask);
    context->e0 = _mm_add_epi32(context->e0, src0_4x);
    e1 = context->abcd;
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, context->e0, 0);

    // NOTE(michiel): 4 - 8 / 80
    __m128i src1_4x = _mm_loadu_si128(src + 1);
    src1_4x = _mm_shuffle_epi8(src1_4x, context->mask);
    e1 = _mm_sha1nexte_epu32(e1, src1_4x);
    context->e0 = context->abcd;
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, e1, 0);
    src0_4x = _mm_sha1msg1_epu32(src0_4x, src1_4x);

    // NOTE(michiel): 8 - 12 / 80
    __m128i src2_4x = _mm_loadu_si128(src + 2);
    src2_4x = _mm_shuffle_epi8(src2_4x, context->mask);
    context->e0 = _mm_sha1nexte_epu32(context->e0, src2_4x);
    e1 = context->abcd;
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, context->e0, 0);
    src1_4x = _mm_sha1msg1_epu32(src1_4x, src2_4x);
    src0_4x = _mm_xor_si128(src0_4x, src2_4x);

    // NOTE(michiel): 12 - 16 / 80
    __m128i src3_4x = _mm_loadu_si128(src + 3);
    src3_4x = _mm_shuffle_epi8(src3_4x, context->mask);
    e1 = _mm_sha1nexte_epu32(e1, src3_4x);
    context->e0 = context->abcd;
    src0_4x = _mm_sha1msg2_epu32(src0_4x, src3_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, e1, 0);
    src2_4x = _mm_sha1msg1_epu32(src2_4x, src3_4x);
    src1_4x = _mm_xor_si128(src1_4x, src3_4x);

    // NOTE(michiel): 16 - 20 / 80
    context->e0 = _mm_sha1nexte_epu32(context->e0, src0_4x);
    e1 = context->abcd;
    src1_4x = _mm_sha1msg2_epu32(src1_4x, src0_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, context->e0, 0);
    src3_4x = _mm_sha1msg1_epu32(src3_4x, src0_4x);
    src2_4x = _mm_xor_si128(src2_4x, src0_4x);

    // NOTE(michiel): 20 - 24 / 80
    e1 = _mm_sha1nexte_epu32(e1, src1_4x);
    context->e0 = context->abcd;
    src2_4x = _mm_sha1msg2_epu32(src2_4x, src1_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, e1, 1);
    src0_4x = _mm_sha1msg1_epu32(src0_4x, src1_4x);
    src3_4x = _mm_xor_si128(src3_4x, src1_4x);

    // NOTE(michiel): 24 - 28 / 80
    context->e0 = _mm_sha1nexte_epu32(context->e0, src2_4x);
    e1 = context->abcd;
    src3_4x = _mm_sha1msg2_epu32(src3_4x, src2_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, context->e0, 1);
    src1_4x = _mm_sha1msg1_epu32(src1_4x, src2_4x);
    src0_4x = _mm_xor_si128(src0_4x, src2_4x);

    // NOTE(michiel): 28 - 32 / 80
    e1 = _mm_sha1nexte_epu32(e1, src3_4x);
    context->e0 = context->abcd;
    src0_4x = _mm_sha1msg2_epu32(src0_4x, src3_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, e1, 1);
    src2_4x = _mm_sha1msg1_epu32(src2_4x, src3_4x);
    src1_4x = _mm_xor_si128(src1_4x, src3_4x);

    // NOTE(michiel): 32 - 36 / 80
    context->e0 = _mm_sha1nexte_epu32(context->e0, src0_4x);
    e1 = context->abcd;
    src1_4x = _mm_sha1msg2_epu32(src1_4x, src0_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, context->e0, 1);
    src3_4x = _mm_sha1msg1_epu32(src3_4x, src0_4x);
    src2_4x = _mm_xor_si128(src2_4x, src0_4x);

    // NOTE(michiel): 36 - 40 / 80
    e1 = _mm_sha1nexte_epu32(e1, src1_4x);
    context->e0 = context->abcd;
    src2_4x = _mm_sha1msg2_epu32(src2_4x, src1_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, e1, 1);
    src0_4x = _mm_sha1msg1_epu32(src0_4x, src1_4x);
    src3_4x = _mm_xor_si128(src3_4x, src1_4x);

    // NOTE(michiel): 40 - 44 / 80
    context->e0 = _mm_sha1nexte_epu32(context->e0, src2_4x);
    e1 = context->abcd;
    src3_4x = _mm_sha1msg2_epu32(src3_4x, src2_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, context->e0, 2);
    src1_4x = _mm_sha1msg1_epu32(src1_4x, src2_4x);
    src0_4x = _mm_xor_si128(src0_4x, src2_4x);

    // NOTE(michiel): 44 - 48 / 80
    e1 = _mm_sha1nexte_epu32(e1, src3_4x);
    context->e0 = context->abcd;
    src0_4x = _mm_sha1msg2_epu32(src0_4x, src3_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, e1, 2);
    src2_4x = _mm_sha1msg1_epu32(src2_4x, src3_4x);
    src1_4x = _mm_xor_si128(src1_4x, src3_4x);

    // NOTE(michiel): 48 - 52 / 80
    context->e0 = _mm_sha1nexte_epu32(context->e0, src0_4x);
    e1 = context->abcd;
    src1_4x = _mm_sha1msg2_epu32(src1_4x, src0_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, context->e0, 2);
    src3_4x = _mm_sha1msg1_epu32(src3_4x, src0_4x);
    src2_4x = _mm_xor_si128(src2_4x, src0_4x);

    // NOTE(michiel): 52 - 56 / 80
    e1 = _mm_sha1nexte_epu32(e1, src1_4x);
    context->e0 = context->abcd;
    src2_4x = _mm_sha1msg2_epu32(src2_4x, src1_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, e1, 2);
    src0_4x = _mm_sha1msg1_epu32(src0_4x, src1_4x);
    src3_4x = _mm_xor_si128(src3_4x, src1_4x);

    // NOTE(michiel): 56 - 60 / 80
    context->e0 = _mm_sha1nexte_epu32(context->e0, src2_4x);
    e1 = context->abcd;
    src3_4x = _mm_sha1msg2_epu32(src3_4x, src2_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, context->e0, 2);
    src1_4x = _mm_sha1msg1_epu32(src1_4x, src2_4x);
    src0_4x = _mm_xor_si128(src0_4x, src2_4x);

    // NOTE(michiel): 60 - 64 / 80
    e1 = _mm_sha1nexte_epu32(e1, src3_4x);
    context->e0 = context->abcd;
    src0_4x = _mm_sha1msg2_epu32(src0_4x, src3_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, e1, 3);
    src2_4x = _mm_sha1msg1_epu32(src2_4x, src3_4x);
    src1_4x = _mm_xor_si128(src1_4x, src3_4x);

    // NOTE(michiel): 64 - 68 / 80
    context->e0 = _mm_sha1nexte_epu32(context->e0, src0_4x);
    e1 = context->abcd;
    src1_4x = _mm_sha1msg2_epu32(src1_4x, src0_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, context->e0, 3);
    src3_4x = _mm_sha1msg1_epu32(src3_4x, src0_4x);
    src2_4x = _mm_xor_si128(src2_4x, src0_4x);

    // NOTE(michiel): 68 - 72 / 80
    e1 = _mm_sha1nexte_epu32(e1, src1_4x);
    context->e0 = context->abcd;
    src2_4x = _mm_sha1msg2_epu32(src2_4x, src1_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, e1, 3);
    src3_4x = _mm_xor_si128(src3_4x, src1_4x);

    // NOTE(michiel): 72 - 76 / 80
    context->e0 = _mm_sha1nexte_epu32(context->e0, src2_4x);
    e1 = context->abcd;
    src3_4x = _mm_sha1msg2_epu32(src3_4x, src2_4x);
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, context->e0, 3);

    // NOTE(michiel): 76 - 80 / 80
    e1 = _mm_sha1nexte_epu32(e1, src3_4x);
    context->e0 = context->abcd;
    context->abcd = _mm_sha1rnds4_epu32(context->abcd, e1, 3);

    context->e0 = _mm_sha1nexte_epu32(context->e0, origE0);
    context->abcd = _mm_add_epi32(context->abcd, origAbcd);
}

internal void
sha1_padding(Sha1Context *context, umm srcCount, u8 *src,
             u8 *chunk, umm totalByteCount)
{
    // NOTE(michiel): chunk[64] and srcCount < 64
    copy(srcCount, src, chunk);
    chunk[srcCount++] = 0x80;

    b32 doubleEnd = srcCount > 56;
    copy_single((doubleEnd ? 64 : 56) - srcCount, 0, chunk + srcCount);
    if (doubleEnd)
    {
        sha1_process_chunk(context, chunk);
        copy_single(56, 0, chunk);
    }

    totalByteCount *= 8;
    chunk[56] = (totalByteCount >> 56) & 0xFF;
    chunk[57] = (totalByteCount >> 48) & 0xFF;
    chunk[58] = (totalByteCount >> 40) & 0xFF;
    chunk[59] = (totalByteCount >> 32) & 0xFF;
    chunk[60] = (totalByteCount >> 24) & 0xFF;
    chunk[61] = (totalByteCount >> 16) & 0xFF;
    chunk[62] = (totalByteCount >>  8) & 0xFF;
    chunk[63] = (totalByteCount >>  0) & 0xFF;
}

internal void
sha1_digest(umm byteCount, u8 *bytes, u32 *output)
{
    Sha1Context sha1context = sha1_initialize();

    u64 srcCount = byteCount;
    u8 *src = bytes;

    while (srcCount > 63)
    {
        sha1_process_chunk(&sha1context, src);
        srcCount -= 64;
        src      += 64;
    }

    u8 chunk[64];
    sha1_padding(&sha1context, srcCount, src, chunk, byteCount);
    sha1_process_chunk(&sha1context, chunk);

    sha1_finish(&sha1context, output);
}

//
// NOTE(michiel): SHA256
//

internal Sha256Context
sha256_initialize(void)
{
    Sha256Context result = {};
    result.mask = _mm_set_epi64x(0x0C0D0E0F08090A0BULL, 0x0405060700010203ULL);
    result.abef = _mm_set_epi32(0x6A09E667, 0xBB67AE85, 0x510E527F, 0x9B05688C);
    result.cdgh = _mm_set_epi32(0x3C6EF372, 0xA54FF53A, 0x1F83D9AB, 0x5BE0CD19);

    return result;
}

internal void
sha256_finish(Sha256Context *context, u32 *output)
{
    // NOTE(michiel): expect output[8]
    context->abef = _mm_shuffle_epi8(context->abef, context->mask);
    context->cdgh = _mm_shuffle_epi8(context->cdgh, context->mask);
    output[0] = _mm_extract_epi32(context->abef, 3);
    output[1] = _mm_extract_epi32(context->abef, 2);
    output[2] = _mm_extract_epi32(context->cdgh, 3);
    output[3] = _mm_extract_epi32(context->cdgh, 2);
    output[4] = _mm_extract_epi32(context->abef, 1);
    output[5] = _mm_extract_epi32(context->abef, 0);
    output[6] = _mm_extract_epi32(context->cdgh, 1);
    output[7] = _mm_extract_epi32(context->cdgh, 0);
}

internal void
sha256_process_chunk(Sha256Context *context, u8 *chunk)
{
    // NOTE(michiel): https://software.intel.com/en-us/articles/intel-sha-extensions
    // NOTE(michiel): expect chunk[64]

    persist const u32 ks[64] = {
        0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
        0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
        0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
        0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
        0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
        0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
        0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
        0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
        0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
        0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
        0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
        0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
        0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
        0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
        0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
        0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
    };

    __m128i *src128 = (__m128i *)chunk;
    __m128i *ks128  = (__m128i *)ks;

    __m128i origAbef = context->abef;
    __m128i origCdgh = context->cdgh;

    __m128i ks_4x;
    __m128i src0_4x;
    __m128i src1_4x;
    __m128i src2_4x;
    __m128i src3_4x;
    __m128i src4_4x;

    // NOTE(michiel): 0 - 4 / 64
    src0_4x  = _mm_loadu_si128(src128 + 0);
    ks_4x    = _mm_loadu_si128(ks128  + 0);

    src0_4x  = _mm_shuffle_epi8(src0_4x, context->mask);
    ks_4x    = _mm_add_epi32(ks_4x, src0_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);

    // NOTE(michiel): 4 - 8 / 64
    src1_4x  = _mm_loadu_si128(src128 + 1);
    ks_4x    = _mm_loadu_si128(ks128  + 1);

    src1_4x  = _mm_shuffle_epi8(src1_4x, context->mask);
    ks_4x    = _mm_add_epi32(ks_4x, src1_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src0_4x  = _mm_sha256msg1_epu32(src0_4x, src1_4x);

    // NOTE(michiel): 8 - 12 / 64
    src2_4x  = _mm_loadu_si128(src128 + 2);
    ks_4x    = _mm_loadu_si128(ks128  + 2);

    src2_4x  = _mm_shuffle_epi8(src2_4x, context->mask);
    ks_4x    = _mm_add_epi32(ks_4x, src2_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src1_4x  = _mm_sha256msg1_epu32(src1_4x, src2_4x);

    // NOTE(michiel): 12 - 16 / 64
    src3_4x  = _mm_loadu_si128(src128 + 3);
    ks_4x    = _mm_loadu_si128(ks128  + 3);

    src3_4x  = _mm_shuffle_epi8(src3_4x, context->mask);
    ks_4x    = _mm_add_epi32(ks_4x, src3_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src3_4x, src2_4x, 4);
    src0_4x = _mm_add_epi32(src0_4x, src4_4x);
    src0_4x = _mm_sha256msg2_epu32(src0_4x, src3_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src2_4x = _mm_sha256msg1_epu32(src2_4x, src3_4x);

#if 0
#define do_round(i, y, x, z) \
    ks_4x    = _mm_loadu_si128(ks128 + i); \
    ks_4x    = _mm_add_epi32(ks_4x, x); \
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x); \
    src4_4x  = _mm_alignr_epi8(x, y, 4); \
    z        = _mm_add_epi32(z, src4_4x); \
    z        = _mm_sha256msg2_epu32(z, x); \
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E); \
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x); \
    y        = _mm_sha256msg1_epu32(y, x)

    do_round( 4, src3_4x, src0_4x, src1_4x);
    do_round( 5, src0_4x, src1_4x, src2_4x);
    do_round( 6, src1_4x, src2_4x, src3_4x);
    do_round( 7, src2_4x, src3_4x, src0_4x);
    do_round( 8, src3_4x, src0_4x, src1_4x);
    do_round( 9, src0_4x, src1_4x, src2_4x);
    do_round(10, src1_4x, src2_4x, src3_4x);
    do_round(11, src2_4x, src3_4x, src0_4x);
    do_round(12, src3_4x, src0_4x, src1_4x);
#else
    // NOTE(michiel): 16 - 20 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 4);

    ks_4x    = _mm_add_epi32(ks_4x, src0_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src0_4x, src3_4x, 4);
    src1_4x = _mm_add_epi32(src1_4x, src4_4x);
    src1_4x = _mm_sha256msg2_epu32(src1_4x, src0_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src3_4x = _mm_sha256msg1_epu32(src3_4x, src0_4x);

    // NOTE(michiel): 20 - 24 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 5);

    ks_4x    = _mm_add_epi32(ks_4x, src1_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src1_4x, src0_4x, 4);
    src2_4x = _mm_add_epi32(src2_4x, src4_4x);
    src2_4x = _mm_sha256msg2_epu32(src2_4x, src1_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src0_4x = _mm_sha256msg1_epu32(src0_4x, src1_4x);

    // NOTE(michiel): 24 - 28 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 6);

    ks_4x    = _mm_add_epi32(ks_4x, src2_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src2_4x, src1_4x, 4);
    src3_4x = _mm_add_epi32(src3_4x, src4_4x);
    src3_4x = _mm_sha256msg2_epu32(src3_4x, src2_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src1_4x = _mm_sha256msg1_epu32(src1_4x, src2_4x);

    // NOTE(michiel): 28 - 32 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 7);

    ks_4x    = _mm_add_epi32(ks_4x, src3_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src3_4x, src2_4x, 4);
    src0_4x = _mm_add_epi32(src0_4x, src4_4x);
    src0_4x = _mm_sha256msg2_epu32(src0_4x, src3_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src2_4x = _mm_sha256msg1_epu32(src2_4x, src3_4x);

    // NOTE(michiel): 32 - 36 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 8);

    ks_4x    = _mm_add_epi32(ks_4x, src0_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src0_4x, src3_4x, 4);
    src1_4x = _mm_add_epi32(src1_4x, src4_4x);
    src1_4x = _mm_sha256msg2_epu32(src1_4x, src0_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src3_4x = _mm_sha256msg1_epu32(src3_4x, src0_4x);

    // NOTE(michiel): 36 - 40 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 9);

    ks_4x    = _mm_add_epi32(ks_4x, src1_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src1_4x, src0_4x, 4);
    src2_4x = _mm_add_epi32(src2_4x, src4_4x);
    src2_4x = _mm_sha256msg2_epu32(src2_4x, src1_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src0_4x = _mm_sha256msg1_epu32(src0_4x, src1_4x);

    // NOTE(michiel): 40 - 44 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 10);

    ks_4x    = _mm_add_epi32(ks_4x, src2_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src2_4x, src1_4x, 4);
    src3_4x = _mm_add_epi32(src3_4x, src4_4x);
    src3_4x = _mm_sha256msg2_epu32(src3_4x, src2_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src1_4x = _mm_sha256msg1_epu32(src1_4x, src2_4x);

    // NOTE(michiel): 44 - 48 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 11);

    ks_4x    = _mm_add_epi32(ks_4x, src3_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src3_4x, src2_4x, 4);
    src0_4x = _mm_add_epi32(src0_4x, src4_4x);
    src0_4x = _mm_sha256msg2_epu32(src0_4x, src3_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src2_4x = _mm_sha256msg1_epu32(src2_4x, src3_4x);

    // NOTE(michiel): 48 - 52 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 12);

    ks_4x    = _mm_add_epi32(ks_4x, src0_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src0_4x, src3_4x, 4);
    src1_4x = _mm_add_epi32(src1_4x, src4_4x);
    src1_4x = _mm_sha256msg2_epu32(src1_4x, src0_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);
    src3_4x = _mm_sha256msg1_epu32(src3_4x, src0_4x);
#endif

    // NOTE(michiel): 52 - 56 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 13);

    ks_4x    = _mm_add_epi32(ks_4x, src1_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src1_4x, src0_4x, 4);
    src2_4x = _mm_add_epi32(src2_4x, src4_4x);
    src2_4x = _mm_sha256msg2_epu32(src2_4x, src1_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);

    // NOTE(michiel): 56 - 60 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 14);

    ks_4x    = _mm_add_epi32(ks_4x, src2_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    src4_4x = _mm_alignr_epi8(src2_4x, src1_4x, 4);
    src3_4x = _mm_add_epi32(src3_4x, src4_4x);
    src3_4x = _mm_sha256msg2_epu32(src3_4x, src2_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);

    // NOTE(michiel): 60 - 64 / 64
    ks_4x    = _mm_loadu_si128(ks128 + 15);

    ks_4x    = _mm_add_epi32(ks_4x, src3_4x);
    context->cdgh = _mm_sha256rnds2_epu32(context->cdgh, context->abef, ks_4x);
    ks_4x    = _mm_shuffle_epi32(ks_4x, 0x0E);
    context->abef = _mm_sha256rnds2_epu32(context->abef, context->cdgh, ks_4x);

    context->abef = _mm_add_epi32(context->abef, origAbef);
    context->cdgh = _mm_add_epi32(context->cdgh, origCdgh);
}

internal void
sha256_padding(Sha256Context *context, umm srcCount, u8 *src,
               u8 *chunk, umm totalByteCount)
{
    // NOTE(michiel): chunk[64] and srcCount < 64
    copy(srcCount, src, chunk);
    chunk[srcCount++] = 0x80;

    b32 doubleEnd = srcCount > 56;
    copy_single((doubleEnd ? 64 : 56) - srcCount, 0, chunk + srcCount);
    if (doubleEnd)
    {
        sha256_process_chunk(context, chunk);
        copy_single(56, 0, chunk);
    }

    totalByteCount *= 8;
    chunk[56] = (totalByteCount >> 56) & 0xFF;
    chunk[57] = (totalByteCount >> 48) & 0xFF;
    chunk[58] = (totalByteCount >> 40) & 0xFF;
    chunk[59] = (totalByteCount >> 32) & 0xFF;
    chunk[60] = (totalByteCount >> 24) & 0xFF;
    chunk[61] = (totalByteCount >> 16) & 0xFF;
    chunk[62] = (totalByteCount >>  8) & 0xFF;
    chunk[63] = (totalByteCount >>  0) & 0xFF;
}

internal void
sha256_digest(umm byteCount, u8 *bytes, u32 *output)
{
    Sha256Context sha256Context = sha256_initialize();

    u64 srcCount = byteCount;
    u8 *src = bytes;

    while (srcCount > 63)
    {
        sha256_process_chunk(&sha256Context, src);
        srcCount -= 64;
        src      += 64;
    }

    u8 chunk[64];
    sha256_padding(&sha256Context, srcCount, src, chunk, byteCount);
    sha256_process_chunk(&sha256Context, chunk);

    sha256_finish(&sha256Context, output);
}
