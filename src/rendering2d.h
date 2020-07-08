struct DrawClear
{
    v4 colour;
};

struct DrawImage
{
    Image bitmap;
};

enum DrawKind
{
    Draw_None,
    Draw_Clear,
    Draw_Line,
    Draw_Triangle,
    Draw_Circle,
    Draw_Image,
};
struct DrawHeader
{
    DrawKind kind;
    umm byteSize;
};

struct Renderer2D
{
    u32 width;
    u32 height;

    v4 background;

    u32 maxVertexCount;
    u32 vertexCount;
    Vertex2D *vertexBuffer;

    u32 maxTextureByteCount;
    u32 textureByteCount;
    u32* textureBuffer;

    // NOTE(michiel): Size in bytes so not to overflow the buffer
    u32 maxDrawSize;

    // NOTE(michiel): Count for looping over all headers and extracting the data
    u32 drawCount; // NOTE(michiel): Amount of expected headers in the buffer
    u8 *drawBuffer;
    u8 *drawBufferAt;
};

#define INIT_RENDERER(name) Renderer2D name(API *api, Arena *memory, u32 width, u32 height, u32 maxVertexCount, u32 maxTextureByteCount, u32 maxDrawSize)
typedef INIT_RENDERER(InitRenderer);

#define RENDER_DISPLAY(name) void name(API *api, Image *image, Renderer2D *renderer)
typedef RENDER_DISPLAY(RenderDisplay);

//
// NOTE(michiel): Push to render buffer calls
//

internal DrawHeader *
push_header(Renderer2D *renderer, DrawKind kind, umm reserveSize = 0)
{
    umm newSize = sizeof(DrawHeader) + reserveSize;
    //newSize = (newSize + 0x7) & ~0x7;
    i_expect((renderer->drawBufferAt - renderer->drawBuffer + newSize) < renderer->maxDrawSize);

    DrawHeader *result = (DrawHeader *)renderer->drawBufferAt;
    result->kind = kind;
    result->byteSize = newSize;
    renderer->drawBufferAt += newSize;
    ++renderer->drawCount;
    return result;
}

internal Vertex2D *
push_vertex(Renderer2D *renderer, Vertex2D vertex)
{
    i_expect(renderer->vertexCount < renderer->maxVertexCount);
    Vertex2D *result = renderer->vertexBuffer + renderer->vertexCount++;
    *result = vertex;
    return result;
}

internal u32 *
push_texture(Renderer2D *renderer, umm size)
{
    i_expect(renderer->textureByteCount + size <= renderer->maxTextureByteCount);
    u32 *result = renderer->textureBuffer + renderer->textureByteCount;
    renderer->textureByteCount += size;
    return result;
}

internal void
push_clear(Renderer2D *renderer, v4 colour)
{
    DrawHeader *result = push_header(renderer, Draw_Clear, sizeof(DrawClear));
    DrawClear *clear = (DrawClear *)(result + 1);
    clear->colour = colour;
}

internal void
push_line(Renderer2D *renderer, v2 start, v2 end, v4 colour)
{
    Vertex2D startV = {start, colour};
    Vertex2D endV = {end, colour};
    DrawHeader *result = push_header(renderer, Draw_Line);
    push_vertex(renderer, startV);
    push_vertex(renderer, endV);
}

internal void
push_rectangle(Renderer2D *renderer, v2 offset, v2 size, v4 colour)
{
    Vertex2D one = {offset, colour};
    Vertex2D two = {V2(offset.x, offset.y + size.y - 1), colour};
    Vertex2D three = {V2(offset.x + size.x - 1, offset.y + size.y - 1), colour};
    Vertex2D four = {V2(offset.x + size.x - 1, offset.y), colour};

    DrawHeader *result = push_header(renderer, Draw_Triangle);
    push_vertex(renderer, one);
    push_vertex(renderer, two);
    push_vertex(renderer, three);
    result = push_header(renderer, Draw_Triangle);
    push_vertex(renderer, one);
    push_vertex(renderer, three);
    push_vertex(renderer, four);
}

internal void
push_triangle(Renderer2D *renderer, v2 a, v2 b, v2 c, v4 colour)
{
    DrawHeader *result = push_header(renderer, Draw_Triangle);
    push_vertex(renderer, (Vertex2D){a, colour});
    push_vertex(renderer, (Vertex2D){b, colour});
    push_vertex(renderer, (Vertex2D){c, colour});
}

internal void
push_circle(Renderer2D *renderer, v2 center, f32 radius, v4 colour)
{
    DrawHeader *result = push_header(renderer, Draw_Circle);
    push_vertex(renderer, (Vertex2D){center, colour});
    push_vertex(renderer, (Vertex2D){V2(radius, 0), colour});
}

internal void
push_circle_gradient(Renderer2D *renderer, v2 center, f32 radius, v4 colour, v4 edgeColour,
                     f32 innerRadius = 0.0f)
{
    DrawHeader *result = push_header(renderer, Draw_Circle);
    push_vertex(renderer, (Vertex2D){center, colour});
    push_vertex(renderer, (Vertex2D){V2(radius, innerRadius), edgeColour});
}

internal void
push_image(Renderer2D *renderer, v2 offset, Image *image,
           v4 colour = V4(1, 1, 1, 1))
{
    umm imageByteSize = image->width * image->height * 4;
    DrawHeader *result = push_header(renderer, Draw_Image, sizeof(DrawImage));
    DrawImage *drawImage = (DrawImage *)(result + 1);
    push_vertex(renderer, (Vertex2D){offset, colour});
    drawImage->bitmap.width = image->width;
    drawImage->bitmap.height = image->height;
    drawImage->bitmap.rowStride = image->width;
    drawImage->bitmap.pixels = push_texture(renderer, imageByteSize);
    u32 *srcRow = image->pixels;
    u32 *dstRow = drawImage->bitmap.pixels;
    for (u32 y = 0; y < image->height; ++y)
    {
        copy(sizeof(u32)*image->width, srcRow, dstRow);
        srcRow += image->rowStride;
        dstRow += drawImage->bitmap.rowStride;
    }
}

#if 0
internal void
push_tube(Renderer2D *renderer, v2 offset, v2 size, v4 colour, v4 edgeColour)
{
    // TODO(michiel): Two rectangles with a gradient
    DrawHeader *result = push_header(renderer);
    result->kind = Draw_Tube;
    result->tube.offset = offset;
    result->tube.size = size;
    result->tube.colour = colour;
    result->tube.edgeColour = edgeColour;
}

internal void
push_text(Renderer2D *renderer, v2 offset, u32 maxWidth, String text,
          v4 colour = V4(1, 1, 1, 1),
          FontSize size = FontSize_Medium, FontAlignment align = FontAlign_Left)
{
    // TODO(michiel): Move creation of image to push call and have an image buffer as well as a vertex buffer
    DrawHeader *result = push_header(renderer, Draw_Text, sizeof(DrawText) + text.size + 1);
    DrawText *drawText = (DrawText *)(result + 1);
    push_vertex(renderer, (Vertex2D){offset, colour});
    drawText->maxWidth = maxWidth;
    drawText->flags = (align & FontAlign_MASK) | (size & FontSize_MASK);
    drawText->text.size = text.size;
    drawText->text.data = (u8 *)(drawText + 1);
    copy(text.size, text.data, drawText->text.data);
    drawText->text.data[text.size] = 0;
}

internal void
push_text_fmt(Renderer2D *renderer, v2 offset, u32 maxWidth, v4 colour,
              FontSize size, FontAlignment align,
              char *fmt, ...)
{
    u8 textBuffer[1024];
    va_list args;
    va_start(args, fmt);
    String formatted = vstring_fmt(array_count(textBuffer), textBuffer, fmt, args);
    va_end(args);
    push_text(renderer, offset, maxWidth, formatted, colour, size, align);
}

internal void
push_text(Renderer2D *renderer, v2 offset, u32 maxWidth, char *text,
          v4 colour = V4(1, 1, 1, 1),
          FontSize size = FontSize_Medium, FontAlignment align = FontAlign_Left)
{
    push_text(renderer, offset, maxWidth, string(text), colour, size, align);
}
#endif

