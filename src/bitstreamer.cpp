internal u64
get_bits_be(BitStreamer *stream, u32 nrBits)
{
    // NOTE(michiel): This gets the big endian integer specified by nrBits
    i_expect(nrBits <= 64);
    i_expect(nrBits <= ((stream->end - stream->at) << 3));

    u64 result = stream->remainingData;
    if (nrBits == stream->remainingBits)
    {
        stream->remainingBits = 0;
        stream->remainingData = 0;
    }
    else if (nrBits < stream->remainingBits)
    {
        stream->remainingBits -= nrBits;
        result >>= stream->remainingBits;
        stream->remainingData &= (1 << stream->remainingBits) - 1;
    }
    else
    {
        u32 bytes = nrBits >> 3;
        u32 remaining = nrBits & 0x7;

        if (remaining == stream->remainingBits)
        {
            remaining = 0;
        }
        else if (remaining < stream->remainingBits)
        {
            i_expect(bytes > 0);
            bytes -= 1;
            remaining = (remaining + 8) - stream->remainingBits;
        }
        else
        {
            i_expect(remaining > stream->remainingBits);
            remaining -= stream->remainingBits;
        }
        stream->remainingBits = 0;
        stream->remainingData = 0;

        for (u32 byte = 0; byte < bytes; ++byte)
        {
            result <<= 8;
            result |= *stream->at++;
        }

        if (remaining)
        {
            result <<= remaining;
            stream->remainingData = *stream->at++;
            stream->remainingBits = 8 - remaining;
            result |= stream->remainingData >> stream->remainingBits;
            stream->remainingData &= (1 << stream->remainingBits) - 1;
        }
    }

    return result;
}

internal u64
get_bits_le(BitStreamer *stream, u32 nrBits)
{
    // NOTE(michiel): This gets the big endian integer specified by nrBits
    i_expect(nrBits <= 64);
    i_expect(nrBits <= ((stream->end - stream->at) << 3));

    // TODO(michiel): testing
    u64 result = stream->remainingData;
    if (nrBits == stream->remainingBits)
    {
        stream->remainingBits = 0;
        stream->remainingData = 0;
    }
    else if (nrBits < stream->remainingBits)
    {
        stream->remainingBits -= nrBits;
        result &= (1 << nrBits) - 1;
        stream->remainingData >>= nrBits;
    }
    else
    {
        u32 bytes = nrBits >> 3;
        u32 remaining = nrBits & 0x7;

        // TODO(michiel): testing these cases
        u32 shiftUp = stream->remainingBits;
        if (remaining == stream->remainingBits)
        {
            remaining = 0;
        }
        else if (remaining < stream->remainingBits)
        {
            i_expect(bytes > 0);
            bytes -= 1;
            remaining = (remaining + 8) - stream->remainingBits;
        }
        else
        {
            i_expect(remaining > stream->remainingBits);
            remaining -= stream->remainingBits;
        }
        stream->remainingBits = 0;
        stream->remainingData = 0;

        result |= ((*(u64 *)stream->at) & ((1 << nrBits) - 1)) << shiftUp;
        stream->at += bytes;

        if (remaining)
        {
            stream->remainingData = *stream->at++;
            stream->remainingBits = 8 - remaining;
            result |= (stream->remainingData & ((1 << remaining) - 1)) << ((bytes * 8) + shiftUp);
            stream->remainingData >>= remaining;
        }
    }

    return result;
}

internal u64
get_bits(BitStreamer *bitStream, u32 nrBits)
{
    u64 result = 0;
    switch (bitStream->kind) {
        case BitStream_BigEndian: {
            result = get_bits_be(bitStream, nrBits);
        } break;

        case BitStream_LittleEndian: {
            result = get_bits_le(bitStream, nrBits);
        } break;

        default: {} break;
    }
    return result;
}

internal u32
get_le_u32(BitStreamer *bitStream)
{
    i_expect(bitStream->remainingBits == 0);
    u32 result = *(u32 *)bitStream->at;
    bitStream->at += 4;
    return result;
}

internal void
void_bits(BitStreamer *bitStream, u32 nrBits)
{
    u32 sixtyFours = nrBits / 64;
    u32 remaining = nrBits % 64;
    for (u32 x = 0; x < sixtyFours; ++x)
    {
        (void)get_bits(bitStream, 64);
    }
    (void)get_bits(bitStream, remaining);
}

internal void
void_bytes(BitStreamer *bitStream, u32 nrBytes)
{
    void_bits(bitStream, nrBytes * 8);
}

internal Buffer
copy_to_bytes(BitStreamer *bitStream, u32 nrBytes, u8 *dest)
{
    Buffer result = {};
    result.size = nrBytes;
    result.data = dest;
    if (bitStream->remainingBits == 0)
    {
        i_expect((bitStream->end - bitStream->at) >= nrBytes);
        copy(nrBytes, bitStream->at, result.data);
        bitStream->at += nrBytes;
    }
    else
    {
        for (u32 i = 0; i < nrBytes; ++i)
        {
            result.data[i] = get_bits(bitStream, 8);
        }
    }
    return result;
}

internal String
copy_to_string(BitStreamer *bitStream, u32 nrBytes, u8 *dest)
{
    String result = copy_to_bytes(bitStream, nrBytes, dest);
    return result;
}

internal BitStreamer
create_bitstreamer(Buffer memory, BitStreamKind kind /* = BitStream_LittleEndian */)
{
    BitStreamer result = {};
    result.kind = kind;
    result.at = memory.data;
    result.end = memory.data + memory.size;
    return result;
}
