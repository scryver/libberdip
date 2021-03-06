#if _MSC_VER
#undef NO_INTRINSICS
#define NO_INTRINSICS 1
#define _CRT_SECURE_NO_WARNINGS
#else
#include <sys/mman.h>
#endif

#include <stdio.h>

#include "platform.h"
#include "std_memory.h"

global MemoryAPI *gMemoryApi;
//#include "base.h"

#define STBTT_sqrt(x)    square_root(x)
#define STBTT_assert(x)  i_expect_simple(x)
#if COMPILER_MSVC
#define STBTT_ifloor(x)  ((int)floor(x))
#define STBTT_iceil(x)   ((int)ceil(x))
#define STBTT_pow(x, y)  powf(x, y)
#define STBTT_fmod(x, y) fmodf(x, y)
#define STBTT_cos(x)     cosf(x)
#define STBTT_acos(x)    acosf(x)
#define STBTT_fabs(x)    fabs(x)
#else
#define STBTT_ifloor(x)  ((int)__builtin_floor(x))
#define STBTT_iceil(x)   ((int)__builtin_ceil(x))
#define STBTT_pow(x, y)  __builtin_powf(x, y)
#define STBTT_fmod(x, y) __builtin_fmodf(x, y)
#define STBTT_cos(x)     __builtin_cosf(x)
#define STBTT_acos(x)    __builtin_acosf(x)
#define STBTT_fabs(x)    __builtin_fabs(x)
#endif

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define ONE_PAST_MAX_FONT_CODEPOINT (0x10FFFF + 1)

struct FontLoader
{
    FontInfo info;

    u32 maxGlyphCount;
    FontGlyph *glyphs;

    f32 *horizontalAdvance; // NOTE(michiel): Kerning

    u32 *unicodeMap;
};

#include "memory.cpp"
#include "std_memory.cpp"
#include "std_file.c"
#include "bitmap.cpp"
#include "fonts_gb2312.cpp"

internal void
print_usage(char *progName)
{
    fprintf(stdout, "Usage: %s input.ttf output.font\n", progName);
    fprintf(stdout, "       input.ttf   - input true type file\n");
    fprintf(stdout, "       output.font - output font file\n");
    fprintf(stdout, "\n");
}

internal Image
load_glyph_bitmap(MemoryAllocator *allocator, stbtt_fontinfo *fontInfo, FontLoader *font, u32 codePoint, u32 glyphIndex, s32 *xOffset, s32 *yOffset)
{
    Image result = {};

    s32 width, height;
    f32 scale = stbtt_ScaleForPixelHeight(fontInfo, (f32)font->info.pixelHeight);
    u8 *monoBitmap = stbtt_GetCodepointBitmap(fontInfo, scale, scale, (s32)codePoint,
                                              &width, &height, xOffset, yOffset);

    result.width = width + 2;
    result.height = height + 2;
    result.rowStride = width + 2;

    result.pixels = allocate_array(allocator, u32, result.height * result.width, Memory_NoClear);

    fprintf(stderr, "Bitmap '%c' size: %d x %d\n", codePoint, result.width, result.height);

    u8 *source = monoBitmap;
    u32 *destRow = result.pixels + result.rowStride + 1; // + (result.height - 1) * pitch;

    for (s32 y = 1; y < (s32)(result.height - 1); ++y)
    {
        u32 *dest = destRow;
        for (s32 x = 1; x < (s32)(result.width - 1); ++x)
        {
            f32 gray = (f32)(*source++ & 0xFF);
            v4 texel = {255.0f, 255.0f, 255.0f, gray};
            texel = linear1_from_sRGB255(texel);
            texel.rgb *= texel.a;
            texel = sRGB255_from_linear1(texel);

            *dest++ = (((u32)(texel.a + 0.5f) << 24) |
                       ((u32)(texel.r + 0.5f) << 16) |
                       ((u32)(texel.g + 0.5f) << 8) |
                       ((u32)(texel.b + 0.5f) << 0));
        }

        //destRow -= pitch;
        destRow += result.rowStride;
    }


    s32 advance, lsb;
    stbtt_GetCodepointHMetrics(fontInfo, (s32)codePoint, &advance, &lsb);
    f32 charAdvance = (f32)advance * scale;

    f32 kerningChange = (f32)lsb * scale;
    for (u32 otherGlyphIndex = 0; otherGlyphIndex < font->maxGlyphCount; ++otherGlyphIndex)
    {
        font->horizontalAdvance[glyphIndex * font->maxGlyphCount + otherGlyphIndex] += charAdvance - kerningChange;
        if (otherGlyphIndex != 0)
        {
            font->horizontalAdvance[otherGlyphIndex * font->maxGlyphCount + glyphIndex] += kerningChange;
        }
    }

    stbtt_FreeBitmap(monoBitmap, 0);

    return result;
}

internal void
add_character(MemoryAllocator *allocator, stbtt_fontinfo *fontInfo, FontLoader *font, u32 codePoint)
{
    if (stbtt_FindGlyphIndex(fontInfo, codePoint))
    {
        i_expect(font->info.glyphCount < font->maxGlyphCount);

        u32 glyphIndex = font->info.glyphCount++;
        FontGlyph *glyph = font->glyphs + glyphIndex;
        glyph->unicodeCodePoint = codePoint;
        s32 xOffset, yOffset;
        glyph->bitmap = load_glyph_bitmap(allocator, fontInfo, font, codePoint, glyphIndex, &xOffset, &yOffset);
        glyph->yOffset = (f32)yOffset;
        font->unicodeMap[codePoint] = glyphIndex;
        if (font->info.onePastHighestCodePoint <= codePoint)
        {
            font->info.onePastHighestCodePoint = codePoint + 1;
        }
    }
    else
    {
        fprintf(stderr, "Codepoint %u not found!\n", codePoint);
    }
}

int main(int argc, char **argv)
{
    MemoryAPI memApi = {};
    gMemoryApi = &memApi;
    if (argc >= 3)
    {
        char *inputFilename = argv[1];
        char *outputFilename = argv[2];
        s32 pixelHeight = 32;
        if (argc > 3)
        {
            pixelHeight = safe_truncate_to_s32(number_from_string(string(argv[3])));
        }

        MemoryAllocator arenaAllocator = {};
        MemoryArena arena = {};
        std_memory_api(gMemoryApi);
        initialize_arena_allocator(&arena, &arenaAllocator);

        Buffer inFile = read_entire_file(&arenaAllocator, string(inputFilename));
        if (inFile.size != 0)
        {
            stbtt_fontinfo fontInfo = {};
            fprintf(stdout, "Nr fonts found: %d\n", stbtt_GetNumberOfFonts(inFile.data));
            //i_expect(stbtt_GetNumberOfFonts(inFile.data) == 1);

            if (stbtt_InitFont(&fontInfo, inFile.data,
                               stbtt_GetFontOffsetForIndex(inFile.data, 0)))
            {
                ApiFile outFile = open_file(string(outputFilename), FileOpen_Write);

                fprintf(stdout, "Converting '%s' to '%s'...\n", inputFilename, outputFilename);

                FontLoader makeFont = {};
                makeFont.info.pixelHeight = pixelHeight;

                s32 ascent, descent, lineGap;
                //  "*ascent - *descent + *lineGap"
                stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

                f32 scale = stbtt_ScaleForPixelHeight(&fontInfo, (f32)makeFont.info.pixelHeight);
                fprintf(stdout, "Scaling: %f\n", scale);

                makeFont.info.ascenderHeight = (f32)ascent * scale;
                makeFont.info.descenderHeight = (f32)-descent * scale;
                makeFont.info.lineGap = (f32)lineGap * scale;

                makeFont.maxGlyphCount = 0x4000;
                makeFont.info.glyphCount = 0;

                makeFont.unicodeMap = allocate_array(&arenaAllocator, u32, ONE_PAST_MAX_FONT_CODEPOINT, 0);

                makeFont.glyphs = allocate_array(&arenaAllocator, FontGlyph, makeFont.maxGlyphCount, Memory_NoClear);
                umm horizontalAdvances = makeFont.maxGlyphCount * makeFont.maxGlyphCount;
                makeFont.horizontalAdvance = allocate_array(&arenaAllocator, f32, horizontalAdvances, 0);

                makeFont.info.onePastHighestCodePoint = 0;

                // NOTE(michiel): The NULL glyph
                makeFont.info.glyphCount = 1;
                makeFont.glyphs[0].unicodeCodePoint = 0;
                makeFont.glyphs[0].bitmap = {};

                add_character(&arenaAllocator, &fontInfo, &makeFont, ' ');
#if 0
                // TODO(michiel): For now it includes UTF-8 latin and greek
                //for (u32 character = '!'; character <= '~'; ++character)
                for (u32 character = '!'; character < 0x400; ++character)
                {
                    add_character(&arenaAllocator, &fontInfo, &makeFont, character);
                }
#else
                // TODO(michiel): CJK support
                for (u32 character = '!'; character <= '~'; ++character)
                {
                    add_character(&arenaAllocator, &fontInfo, &makeFont, character);
                }

#if 0
                for (u32 charIdx = 0; charIdx < array_count(gGB2312CodePoints); ++charIdx)
                {
                    add_character(&arenaAllocator, &fontInfo, &makeFont, gGB2312CodePoints[charIdx]);
                }
#endif

#if 0
                //for (u32 character = 0x4E00; character < 0x9FEF; ++character)
                for (u32 character = 0x4E00; character < 0x8E00; ++character)
                {
                    add_character(&arenaAllocator, &fontInfo, &makeFont, character);
                }
#endif
#endif

                // NOTE(michiel): Finalize font kerning
                for (u32 a = 0; a < ONE_PAST_MAX_FONT_CODEPOINT; ++a)
                {
                    u32 first = makeFont.unicodeMap[a];
                    if (first)
                    {
                        s32 firstGlyph = stbtt_FindGlyphIndex(&fontInfo, (s32)a);
                        if (firstGlyph)
                        {
                            for (u32 b = 0; b < ONE_PAST_MAX_FONT_CODEPOINT; ++b)
                            {
                                u32 second = makeFont.unicodeMap[b];
                                if (second)
                                {
                                    s32 secondGlyph = stbtt_FindGlyphIndex(&fontInfo, (s32)b);
                                    if (secondGlyph)
                                    {
                                        f32 kerning = (f32)stbtt_GetGlyphKernAdvance(&fontInfo, firstGlyph, secondGlyph) * scale;
                                        makeFont.horizontalAdvance[first * makeFont.maxGlyphCount + second] += kerning;
                                    }
                                }
                            }
                        }
                    }
                }

                // NOTE(michiel): Write font file
                write_to_file(&outFile, sizeof(FontInfo), &makeFont.info);
                write_to_file(&outFile, sizeof(FontGlyph) * makeFont.info.glyphCount, makeFont.glyphs);
                for (u32 glyphIndex = 0; glyphIndex < makeFont.info.glyphCount; ++glyphIndex)
                {
                    FontGlyph *glyph = makeFont.glyphs + glyphIndex;
                    write_to_file(&outFile, sizeof(u32) * glyph->bitmap.width * glyph->bitmap.height, glyph->bitmap.pixels);
                }
                u8 *horizontalAdvance = (u8 *)makeFont.horizontalAdvance;
                for (u32 glyphIndex = 0; glyphIndex < makeFont.info.glyphCount; ++glyphIndex)
                {
                    u32 horizontalAdvanceSliceSize = sizeof(f32) * makeFont.info.glyphCount;
                    write_to_file(&outFile, horizontalAdvanceSliceSize, horizontalAdvance);
                    horizontalAdvance += sizeof(f32) * makeFont.maxGlyphCount;
                }

                close_file(&outFile);
                deallocate_all(&arenaAllocator);
            }
            else
            {
                fprintf(stderr, "Failed to initialize stb truetype\n");
            }
        }
        else
        {
            fprintf(stderr, "Unable to open '%s'\n", inputFilename);
        }
    }
    else
    {
        fprintf(stderr, "Expected 2 arguments, got %d\n", argc - 1);
        print_usage(argv[0]);
    }

    return 0;
}
