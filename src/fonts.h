struct FontGlyph
{
    u32 unicodeCodePoint;
    f32 yOffset;
    Image bitmap;
};

struct FontInfo
{
    s32 pixelHeight;
    
    f32 ascenderHeight;
    f32 descenderHeight;
    f32 lineGap;
    
    u32 glyphCount;
    u32 onePastHighestCodePoint;
};

struct BitmapFont
{
    FontInfo info;
    
    FontGlyph *glyphs; // [glyphCount]
    u16 *unicodeMap; // [onePastHighestCodePoint]
    f32 *horizontalAdvance; // [glyphCount * glyphCount]
};

// NOTE(michiel): Make sure you have an allocate_array(type, count) macro, this will wants some
// memory allocations for the unicodeMap, because it is wasteful to put that into the file
internal void
unpack_font(u8 *fontData, BitmapFont *result)
{
    result->info = *(FontInfo *)fontData;
    umm offset = sizeof(FontInfo);
    result->glyphs = (FontGlyph *)(fontData + offset);
    offset += sizeof(FontGlyph) * result->info.glyphCount;
    
    for (u32 glyphIndex = 0; glyphIndex < result->info.glyphCount; ++glyphIndex)
    {
        FontGlyph *glyph = result->glyphs + glyphIndex;
        glyph->bitmap.pixels = (u32 *)(fontData + offset);
        offset += sizeof(u32) * glyph->bitmap.width * glyph->bitmap.height;
    }
    result->horizontalAdvance = (f32 *)(fontData + offset);
    
    result->unicodeMap = allocate_array(u16, result->info.onePastHighestCodePoint);
    
    for (u32 glyphIndex = 1; glyphIndex < result->info.glyphCount; ++glyphIndex)
    {
        FontGlyph *glyph = result->glyphs + glyphIndex;
        i_expect(glyph->unicodeCodePoint < result->info.onePastHighestCodePoint);
        i_expect((u32)(u16)glyphIndex == glyphIndex);
        result->unicodeMap[glyph->unicodeCodePoint] = (u16)glyphIndex;
    }
}

//
// NOTE(michiel): UTF-8 support
//
#define adv(s, c) *c = (*c << 6) | ((*s++) & ~0xC0)
internal u32
get_code_point_from_utf8(u8 *startOfUtf8, u32 *codePoint)
{
    // NOTE(michiel): Returns amounts of bytes to skip, 0 on malformed encoding.
    u8 *source = startOfUtf8;
    u32 bytes = 0;
    *codePoint = (u32)(*source);
    if ((*codePoint & 0xF8) == 0xF0) {
        // NOTE(michiel): 4-bytes
        *codePoint = (*codePoint & ~0xF8);
        ++source;
        if (*source && ((*source & 0xC0) == 0x80)) {
            adv(source, codePoint);
            if (*source && ((*source & 0xC0) == 0x80)) {
                adv(source, codePoint);
                if (*source && ((*source & 0xC0) == 0x80)) {
                    adv(source, codePoint);
                    bytes = 4;
                }
            }
        }
    } else if ((*codePoint & 0xF0) == 0xE0) {
        // NOTE(michiel): 3-bytes
        *codePoint = (*codePoint & ~0xF0);
        ++source;
        if (*source && ((*source & 0xC0) == 0x80)) {
            adv(source, codePoint);
            if (*source && ((*source & 0xC0) == 0x80)) {
                adv(source, codePoint);
                bytes = 3;
            }
        }
    } else if ((*codePoint & 0xE0) == 0xC0) {
        // NOTE(michiel): 2-bytes
        *codePoint = (*codePoint & ~0xE0);
        ++source;
        if (*source && ((*source & 0xC0) == 0x80)) {
            adv(source, codePoint);
            bytes = 2;
        }
    } else if ((*codePoint & 0x80) == 0) {
        // NOTE(michiel): Single byte (ansii eqv)
        bytes = 1;
    }
    
    return bytes;
}
#undef adv

//
// NOTE(michiel): Font drawing
//

internal u32
get_glyph_from_code_point(BitmapFont *font, u32 codePoint)
{
    u32 result = 0;
    if (codePoint >= font->info.onePastHighestCodePoint)
    {
        codePoint = (u32)(u8)'?';
    }
    result = font->unicodeMap[codePoint];
    if (result >= font->info.glyphCount)
    {
        // TODO(michiel): Add bug report
        result = 0;
    }
    return result;
}

internal f32
get_horizontal_advance_for_pair(BitmapFont *font, u32 prevPoint, u32 codePoint)
{
    u32 prevGlyph = get_glyph_from_code_point(font, prevPoint);
    u32 glyph = get_glyph_from_code_point(font, codePoint);
    
    f32 result = font->horizontalAdvance[prevGlyph * font->info.glyphCount + glyph];
    return result;
}

internal f32
get_line_advance_for(FontInfo *info)
{
    f32 result = info->ascenderHeight + info->descenderHeight + info->lineGap;
    return result;
}

internal f32
get_starting_baseline_y(FontInfo *info)
{
    f32 result = info->ascenderHeight;
    return result;
}
