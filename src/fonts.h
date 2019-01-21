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
// NOTE(michiel): Font drawing
//

internal inline u32
get_glyph_from_code_point(BitmapFont *font, u32 codePoint)
{
    u32 result = 0;
    if (codePoint < font->info.onePastHighestCodePoint)
    {
        result = font->unicodeMap[codePoint];
        i_expect(result < font->info.glyphCount);
    }
    return result;
}

internal f32
get_horizontal_advance_for_pair(BitmapFont *font, u32 prevPoint, u32 codePoint)
{
    i_expect(prevPoint < font->info.onePastHighestCodePoint);
    i_expect(codePoint < font->info.onePastHighestCodePoint);
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
