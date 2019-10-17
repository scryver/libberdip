static void
crc8_init_table(u8 poly, u8 *lut)
{
    // NOTE(michiel): Expects lut[256]
    if (lut)
    {
        for (u32 crcIndex = 0; crcIndex < 256; ++crcIndex)
        {
            u8 crc = crcIndex;
            for (u32 bitIndex = 0; bitIndex < 8; ++bitIndex)
            {
                if (crc & 0x80)
                {
                    crc = (crc << 1) ^ poly;
                }
                else
                {
                    crc = (crc << 1);
                }
            }
            lut[crcIndex] = crc;
        }
    }
}

static u8
crc8_calc_crc(u8 *lut, u32 byteCount, void *data)
{
    u8 result = 0;
    u8 *src = (u8 *)data;
    for (u32 x = 0; x < byteCount; ++x)
    {
        result = lut[result ^ *src++];
    }
    return result;
}

static void
crc16_init_table(u16 poly, u16 *lut)
{
    // NOTE(michiel): Expects lut[256]
    if (lut)
    {
        for (u32 crcIndex = 0; crcIndex < 256; ++crcIndex)
        {
            u16 crc = (crcIndex << 8);
            for (u32 bitIndex = 0; bitIndex < 8; ++bitIndex)
            {
                if (crc & 0x8000)
                {
                    crc = (crc << 1) ^ poly;
                }
                else
                {
                    crc = (crc << 1);
                }
            }
            lut[crcIndex] = crc;
        }
    }
}

static u16
crc16_calc_crc(u16 *lut, u32 byteCount, void *data)
{
    u16 result = 0;
    u8 *src = (u8 *)data;
    for (u32 x = 0; x < byteCount; ++x)
    {
        result = lut[(result >> 8) ^ *src++] ^ (result << 8);
    }
    return result;
}
