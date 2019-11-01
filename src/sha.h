// NOTE(michiel): Compile with the -msha -mssse3 flags

#define SHA1_OUTPUT_COUNT      5
#define SHA1_OUTPUT_BYTE_COUNT (SHA1_OUTPUT_COUNT * sizeof(u32))

struct Sha1Context
{
    __m128i mask;
    __m128i abcd;
    __m128i e0;
};

internal Sha1Context sha1_initialize(void);
internal void sha1_finish(Sha1Context *context, u32 *output);
internal void sha1_process_chunk(Sha1Context *context, u8 *chunk);
internal void sha1_padding(Sha1Context *context, umm srcCount, u8 *src, u8 *chunk, umm totalByteCount);
internal void sha1_digest(umm byteCount, u8 *bytes, u32 *output);

// NOTE(michiel): Usage:
// use the sha1_digest to digest the whole message and put the result in output[5]
// or
// use sha1_initialize
// followed by 0 or more sha1_process_chunk on chunk[64] blocks of data
// followed by sha1_padding on the last < 64 bytes of data, it will append
// the total bit count to the end of the message.
// and finally call sha1_finish with the output[5] array to finalize the digest
/*
 Sha1Context sha1Context = sha1_initialize();

 u32 remainingCount = dataCount;
 u8 *remainingSrc   = data;
while (remainingCount >= 64)
{
sha1_process_chunk(&sha1Context, remainingSrc);
remainingCount -= 64;
remainingSrc   += 64;
}

u8 chunk[64];
sha1_padding(&sha1Context, remainingCount, remainingSrc, chunk, dataCount);

u32 output[5];
sha1_finish(&sha1Context, output);

// Or just in a single call if you don't have a stream of data
u32 output[5];
sha1_digest(dataCount, data, output);

*/

struct Sha256Context
{
    __m128i mask;
    __m128i abef;
    __m128i cdgh;
};

internal Sha256Context sha256_initialize(void);
internal void sha256_finish(Sha256Context *context, u32 *output);
internal void sha256_process_chunk(Sha256Context *context, u8 *chunk);
internal void sha256_padding(Sha256Context *context, umm srcCount, u8 *src, u8 *chunk, umm totalByteCount);
internal void sha256_digest(umm byteCount, u8 *bytes, u32 *output);

// NOTE(michiel): Works the same as the sha1 version, except the output is now 8 u32's.

