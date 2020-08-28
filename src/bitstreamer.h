enum BitStreamKind
{
    BitStream_None,
    BitStream_BigEndian,
    BitStream_LittleEndian,
};
struct BitStreamer
{
    BitStreamKind kind;
    u8 remainingBits;
    u8 remainingData;
    u8 *at;
    u8 *end;
};

internal u64  get_bits(BitStreamer *bitStream, u32 nrBits);
internal void void_bits(BitStreamer *bitStream, u32 nrBits);
internal void void_bytes(BitStreamer *bitStream, u32 nrBytes);
internal Buffer copy_to_bytes(BitStreamer *bitStream, u32 nrBytes, u8 *dest);  // NOTE(michiel): dest[nrBytes]
internal String copy_to_string(BitStreamer *bitStream, u32 nrBytes, u8 *dest);
internal BitStreamer create_bitstreamer(Buffer memory, BitStreamKind kind = BitStream_LittleEndian);
