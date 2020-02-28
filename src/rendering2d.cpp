#include "../libberdip/src/platform.h"
#include "../libberdip/src/suballoc.h"
#include "../libberdip/src/multilane.h"
#include "../libberdip/src/rendering2d.h"

global API *gApi;

//
// NOTE(michiel): STB Truetype related setup code, ignore this if you just want to render text to an image.
//

internal void *
STBTT_memcpy(void *dest, void *src, umm size)
{
    copy(size, src, dest);
    return dest;
}

internal void *
STBTT_memset(void *dest, s32 value, umm size)
{
    copy_single(size, (u32)value, dest);
    return dest;
}

#define stbtt_uint8        u8
#define stbtt_int8         s8
#define stbtt_uint16       u16
#define stbtt_int16        s16
#define stbtt_uint32       u32
#define stbtt_int32        s32

#define STBTT_ifloor(x)    ((s32)floor(x))
#define STBTT_iceil(x)     ((s32)ceil(x))
#define STBTT_sqrt(x)      square_root(x)
#define STBTT_pow(x, y)    pow(x, y)
#define STBTT_fmod(x, y)   modulus(x, y)
#define STBTT_cos(x)       cos(x)
#define STBTT_acos(x)      acos(x)
#define STBTT_fabs(x)      absolute(x)

#define STBTT_malloc(x, u) sub_alloc((SubAllocator *)u, x)
#define STBTT_free(x, u)   sub_dealloc((SubAllocator *)u, x)

#define STBTT_assert(x)    i_expect_simple(x)
//#define STBTT_assert(x)

#define STBTT_strlen(x)    string_length(x)

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "../libberdip/src/stb_truetype.h"

#include "mu_defines.h"

#include "fontloader.h"

#include "../libberdip/src/suballoc.cpp"
#include "../libberdip/src/drawing2d.cpp"

#include "fontloader.cpp"

extern "C" INIT_RENDERER(init_renderer)
{
    gApi = api;

    Renderer2D result = {};
    result.width = width;
    result.height = height;
    u32 fontAllocSize = megabytes(32);
    u8 *fontAllocBuffer = arena_allocate_array(memory, u8, fontAllocSize);
    init_sub_allocator(&result.fontAllocator, fontAllocSize, fontAllocBuffer);

    String regularFont     = static_string("fonts/FreeSans.ttf");
    String boldFont        = static_string("fonts/FreeSansBold.ttf");
    String regularCJKFont  = static_string("fonts/NotoSansCJK-Regular.ttc");
    String boldCJKFont     = static_string("fonts/NotoSansCJK-Bold.ttc");
    String regularThaiFont = static_string("fonts/NotoSansThai-SemiCondensed.ttf");
    String boldThaiFont    = static_string("fonts/NotoSansThai-SemiCondensedBold.ttf");
    String regularDevaFont = static_string("fonts/NotoSansDevanagari-SemiCondensed.ttf");
    String boldDevaFont    = static_string("fonts/NotoSansDevanagari-SemiCondensedBold.ttf");
    String emojiFont       = static_string("fonts/NotoEmoji-Regular.ttf");

    result.font = arena_allocate_struct(memory, StbFont);
    init_stb_font(&result.fontAllocator, result.font);
    add_dual_stb_font(result.font, regularFont, boldFont);
    add_dual_stb_font(result.font, regularCJKFont, boldCJKFont);
    add_dual_stb_font(result.font, regularThaiFont, boldThaiFont);
    add_dual_stb_font(result.font, regularDevaFont, boldDevaFont);
    add_dual_stb_font(result.font, emojiFont, emojiFont);

    result.maxVertexCount = maxVertexCount;
    result.vertexBuffer = arena_allocate_array(memory, Vertex2D, maxVertexCount);

    result.maxTextureByteCount = maxTextureByteCount;
    result.textureBuffer = arena_allocate_array(memory, Vertex2D, maxTextureByteCount);

    result.maxDrawSize = maxDrawSize;
    result.drawBuffer = result.drawBufferAt = arena_allocate_array(memory, u8, maxDrawSize);
    return result;
}

extern "C" RENDER_DISPLAY(render_display)
{
    gApi = api;

    u8 *bufferAt = renderer->drawBuffer;
    Vertex2D *vertexAt = renderer->vertexBuffer;

    for (u32 headerIndex = 0; headerIndex < renderer->drawCount; ++headerIndex)
    {
        DrawHeader *header = (DrawHeader *)bufferAt;
        u8 *dataAt = (u8 *)(header + 1);
        bufferAt += header->byteSize;

        switch(header->kind)
        {
            case Draw_Clear:
            {
                DrawClear *clear = (DrawClear *)dataAt;
                renderer->background = clear->colour;

                Vertex2D one = {V2(0, 0), clear->colour};
                Vertex2D two = {V2(0, (f32)renderer->height), colour};
                Vertex2D three = {V2((f32)renderer->width, (f32)renderer->height), colour};
                Vertex2D four = {V2((f32)renderer->width, 0), colour};
                draw_triangle(image, one, two, three);
                draw_triangle(image, one, three, four);
            } break;

            case Draw_Line:
            {
                //fprintf(stdout, "Line\n");
                Vertex2D start = *vertexAt++;
                Vertex2D end   = *vertexAt++;
                draw_line(image, start, end);
            } break;

            case Draw_Triangle:
            {
                //fprintf(stdout, "Triangle\n");
                Vertex2D a = *vertexAt++;
                Vertex2D b = *vertexAt++;
                Vertex2D c = *vertexAt++;

                draw_triangle(image, a, b, c);
            } break;

            case Draw_Circle:
            {
                //fprintf(stdout, "Circle\n");
                Vertex2D center = *vertexAt++;
                Vertex2D edge = *vertexAt++;
                draw_circle(image, center, edge);
            } break;

            case Draw_Image:
            {
                //fprintf(stdout, "Image\n");
                DrawImage *drawImage = (DrawImage *)dataAt;
                Vertex2D offset = *vertexAt++;

                draw_image(image, offset, &drawImage->bitmap);
            } break;

#if 0
            case Draw_Text:
            {
                //fprintf(stdout, "Text\n");
                DrawText *text = (DrawText *)dataAt;
                Vertex2D offset = *vertexAt++;

                f32 fontHeight = (f32)(text->flags & FontSize_MASK);
                // TODO(michiel): Optimize caching
                TextImage textImage = {};
                create_text_image(renderer->font, text->text, fontHeight, &textImage);

                v2u textSize = V2U(textImage.image.width, textImage.image.height);
                if (textSize.x > text->maxWidth)
                {
                    textSize.x = text->maxWidth;
                }

                if (text->flags & FontAlign_Right)
                {
                    offset.p.x += text->maxWidth - textSize.x;
                }
                else if (text->flags & FontAlign_Center)
                {
                    offset.p.x += (text->maxWidth - textSize.x) / 2;
                }

                draw_image(image, offset, &textImage.image);

                destroy_text_image(renderer->font, &textImage);

            } break;
#endif

            INVALID_DEFAULT_CASE;
        }
    }
}
