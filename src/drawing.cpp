//
// NOTE(michiel): Colour mix helpers
//

internal inline v4 
unpack_colour(u32 colour)
{
    v4 result = {};
    f32 oneOver255 = 1.0f / 255.0f;
    
    result.r = (f32)((colour >>  0) & 0xFF) * oneOver255;
    result.g = (f32)((colour >>  8) & 0xFF) * oneOver255;
    result.b = (f32)((colour >> 16) & 0xFF) * oneOver255;
    result.a = (f32)((colour >> 24) & 0xFF) * oneOver255;
    
    return result;
}

internal inline u32
pack_colour(v4 colour)
{
    u32 result = 0;
    
    result = ((((u32)(clamp01(colour.a) * 255.0f) & 0xFF) << 24) |
              (((u32)(clamp01(colour.b) * 255.0f) & 0xFF) << 16) |
              (((u32)(clamp01(colour.g) * 255.0f) & 0xFF) <<  8) |
              (((u32)(clamp01(colour.r) * 255.0f) & 0xFF) <<  0));
    
    return result;
}

//
// NOTE(michiel): Raw pixel setters
//

internal inline void
draw_pixel(Image *image, u32 x, u32 y, v4 colour)
{
    v4 source = unpack_colour(image->pixels[y * image->width + x]);
    
    colour.r *= colour.a;
    colour.g *= colour.a;
    colour.b *= colour.a;
    
    source.r = source.r * (1.0f - colour.a) + colour.r;
    source.g = source.g * (1.0f - colour.a) + colour.g;
    source.b = source.b * (1.0f - colour.a) + colour.b;
    source.a = colour.a;
    
    image->pixels[y * image->width + x] = pack_colour(source);
}

internal inline void
draw_pixel(Image *image, u32 x, u32 y, u32 colour)
{
    draw_pixel(image, x, y, unpack_colour(colour));
}

//
// NOTE(michiel): Lines
//

internal void
draw_line(Image *image, s32 startX, s32 startY, s32 endX, s32 endY, v4 colour)
{
    startX = minimum(image->width - 1, maximum(0, startX));
    startY = minimum(image->height - 1, maximum(0, startY));
    endX = minimum(image->width - 1, maximum(0, endX));
    endY = minimum(image->height - 1, maximum(0, endY));
    
    b32 yGreaterThanX = false;
    
    s32 absDx = absolute(endX - startX);
    s32 absDy = absolute(endY - startY);
    
    if (absDx < absDy)
    {
        s32 tempS = startX;
        s32 tempE = endX;
        startX = startY;
        endX = endY;
        startY = tempS;
        endY = tempE;
        yGreaterThanX = true;
    }
    
    if (startX > endX)
    {
        s32 tempX = startX;
        s32 tempY = startY;
        startX = endX;
        startY = endY;
        endX = tempX;
        endY = tempY;
    }
    
    s32 dx = endX - startX;
    s32 dy = endY - startY;
    s32 derror2 = 2 * absolute(dy);
    s32 error2 = 0;
    s32 y = startY;
    
    for (s32 x = startX; x < endX; ++x)
    {
        if (yGreaterThanX)
        {
            draw_pixel(image, y, x, colour);
        }
        else
        {
            draw_pixel(image, x, y, colour);
        }
        error2 += derror2;
        if (error2 > dx)
        {
            y += (endY > startY) ? 1 : -1;
            error2 -= dx * 2;
        }
    }
}

internal void
draw_line(Image *image, s32 startX, s32 startY, s32 endX, s32 endY, u32 colour)
{
    draw_line(image, startX, startY, endX, endY, unpack_colour(colour));
}

internal void
draw_line_slow(Image *image, s32 startX, s32 startY, s32 endX, s32 endY, v4 colour)
{
    startX = minimum(image->width - 1, maximum(0, startX));
    startY = minimum(image->height - 1, maximum(0, startY));
    endX = minimum(image->width - 1, maximum(0, endX));
    endY = minimum(image->height - 1, maximum(0, endY));
    
    if (startX > endX)
    {
        s32 tempX = startX;
        s32 tempY = startY;
        startX = endX;
        startY = endY;
        endX = tempX;
        endY = tempY;
    }
    
    s32 dx = endX - startX;
    s32 dy = endY - startY;
    
    s32 absDx = dx;
    if (absDx < 0)
    {
        absDx = -absDx;
    }
    
    s32 absDy = dy;
    if (absDy < 0)
    {
        absDy = -absDy;
    }
    
    s32 steps = maximum(absDx, absDy);
    
    f32 x = startX;
    f32 y = startY;
    
    f32 xInc = (f32)dx / (f32)steps;
    f32 yInc = (f32)dy / (f32)steps;
    
    for (u32 step = 0; step < steps; ++step)
    {
        x += xInc;
        y += yInc;
        u32 xInt = (u32)(x + 0.5f);
        u32 yInt = (u32)(y + 0.5f);
        draw_pixel(image, xInt, yInt, colour);
    }
}

internal void
draw_line_slow(Image *image, s32 startX, s32 startY, s32 endX, s32 endY, u32 colour)
{
    draw_line_slow(image, startX, startY, endX, endY, unpack_colour(colour));
}

internal void
draw_lines(Image *image, u32 pointCount, v2 *points, v4 colour)
{
    i_expect(pointCount);
    v2 prevP = points[0];
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        v2 P = points[pointIdx];
        draw_line(image, round(prevP.x), round(prevP.y), round(P.x), round(P.y), colour);
        prevP = P;
    }
}

internal void
draw_lines(Image *image, u32 pointCount, v2 *points, v2 offset, v2 scale = V2(1, 1),
           v4 colour = V4(1, 1, 1, 1))
{
    i_expect(pointCount);
    v2 prevP = points[0];
    prevP.x *= scale.x;
    prevP.y *= scale.y;
    prevP += offset;
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        v2 P = points[pointIdx];
        P.x *= scale.x;
        P.y *= scale.y;
        P += offset;
        draw_line(image, round(prevP.x), round(prevP.y), round(P.x), round(P.y), colour);
        prevP = P;
    }
}

//
// NOTE(michiel): Outlines
//

internal inline void
outline_rectangle(Image *image, u32 xStart, u32 yStart, u32 width, u32 height, v4 colour)
{
    for (u32 x = xStart; x < (xStart + width); ++x)
    {
        draw_pixel(image, x, yStart, colour);
        draw_pixel(image, x, yStart + height - 1, colour);
    }
    for (u32 y = yStart + 1; y < (yStart + height - 1); ++y)
    {
        draw_pixel(image, xStart, y, colour);
        draw_pixel(image, xStart + width - 1, y, colour);
    }
}

internal inline void
outline_rectangle(Image *image, u32 xStart, u32 yStart, u32 width, u32 height, u32 colour)
{
    outline_rectangle(image, xStart, yStart, width, height, unpack_colour(colour));
}

internal inline void
outline_rectangle(Image *image, Rectangle2u rect, u32 colour)
{
    outline_rectangle(image, rect.min.x, rect.min.y, rect.max.x - rect.min.x, rect.max.y - rect.min.y, colour);
}

internal void
outline_triangle(Image *image, v2u a, v2u b, v2u c, v4 colour)
{
    draw_line(image, a.x, a.y, b.x, b.y, colour);
    draw_line(image, a.x, a.y, c.x, c.y, colour);
    draw_line(image, b.x, b.y, c.x, c.y, colour);
}

internal void
outline_triangle(Image *image, v2u a, v2u b, v2u c, u32 colour)
{
    outline_triangle(image, a, b, c, unpack_colour(colour));
}

internal void
outline_circle(Image *image, s32 xStart, s32 yStart, u32 radius, f32 thickness = 1.0f,
               v4 colour = V4(1, 1, 1, 1))
{
    s32 size = 2 * radius;
    
    f32 r = (s32)radius;
    f32 maxDistSqr = r * r;
    f32 minDistSqr = (r - thickness) * (r - thickness);
    
    xStart = xStart - (s32)radius + 1;
    yStart = yStart - (s32)radius + 1;
    
    for (s32 y = yStart; y < yStart + size; ++y)
    {
        f32 fY = (f32)(y - yStart) - r + 0.5f;
        f32 fYSqr = fY * fY;
        for (s32 x = xStart; x < xStart + size; ++x)
        {
            f32 fX = (f32)(x - xStart) - r + 0.5f;
            f32 distSqr = fX * fX + fYSqr;
            if ((distSqr < maxDistSqr) &&
                (distSqr > minDistSqr) &&
                (0 <= x) && (x < image->width) &&
                (0 <= y) && (y < image->height))
            {
                draw_pixel(image, x, y, colour);
            }
        }
    }
}

internal void
outline_circle(Image *image, s32 xStart, s32 yStart, u32 radius, f32 thickness = 1.0f,
               u32 colour = 0xFFFFFFFF)
{
    outline_circle(image, xStart, yStart, radius, thickness, unpack_colour(colour));
}

//
// NOTE(michiel): Filled shapes, (rect, circle)
//

internal inline void
fill_rectangle(Image *image, s32 xStart, s32 yStart, u32 width, u32 height, v4 colour)
{
    if (xStart < 0)
    {
        s32 diff = -xStart;
        u32 newWidth = width - diff;
        if (newWidth > width)
        {
            newWidth = 0;
        }
        width = newWidth;
        xStart = 0;
    }
    else if ((xStart + width) > image->width)
    {
        s32 diff = (xStart + width) - image->width;
        u32 newWidth = width - diff;
        if (newWidth > width)
        {
            newWidth = 0;
        }
        width = newWidth;
    }
    
    if (yStart < 0)
    {
        s32 diff = -yStart;
        u32 newHeight = height - diff;
        if (newHeight > height)
        {
            newHeight = 0;
        }
        height = newHeight;
        yStart = 0;
    }
    else if ((yStart + height) > image->height)
    {
        s32 diff = (yStart + height) - image->height;
        u32 newHeight = height - diff;
        if (newHeight > height)
        {
            newHeight = 0;
        }
        height = newHeight;
    }
    
    for (u32 y = yStart; y < (yStart + height); ++y)
    {
        for (u32 x = xStart; x < (xStart + width); ++x)
        {
            draw_pixel(image, x, y, colour);
        }
    }
}

internal inline void
fill_rectangle(Image *image, u32 xStart, u32 yStart, u32 width, u32 height, u32 colour)
{
    fill_rectangle(image, xStart, yStart, width, height, unpack_colour(colour));
}

internal inline s32
orient2d(v2s a, v2s b, v2s c)
{
     s32 result = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    return result;
}

internal void
fill_triangle(Image *image, v2s a, v2s b, v2s c, v4 colour)
{
    s32 minX = minimum(a.x, minimum(b.x, c.x));
    s32 minY = minimum(a.y, minimum(b.y, c.y));
    s32 maxX = maximum(a.x, maximum(b.x, c.x));
    s32 maxY = maximum(a.y, maximum(b.y, c.y));
    
    minX = maximum(minX, 0);
    minY = maximum(minY, 0);
    maxX = minimum(maxX, image->width - 1);
    maxY = minimum(maxY, image->height - 1);
    
    s32 A01 = a.y - b.y;
    s32 A12 = b.y - c.y;
    s32 A20 = c.y - a.y;
    
    s32 B01 = b.x - a.x;
    s32 B12 = c.x - b.x;
    s32 B20 = a.x - c.x;
    
    v2s p = V2S(minX, minY);
    
    s32 w0_row = orient2d(b, c, p);
    s32 w1_row = orient2d(c, a, p);
    s32 w2_row = orient2d(a, b, p);
    
    for (p.y = minY; p.y <= maxY; ++p.y)
    {
        s32 w0 = w0_row;
        s32 w1 = w1_row;
        s32 w2 = w2_row;
        
        for (p.x = minX; p.x <= maxX; ++p.x)
        {
            if ((w0 | w1 | w2) >= 0)
            {
                draw_pixel(image, p.x, p.y, colour);
            }
            
            w0 += A12;
            w1 += A20;
            w2 += A01;
        }
        
        w0_row += B12;
        w1_row += B20;
        w2_row += B01;
    }
}

#if 0
internal inline void
fill_triangle(Image *image, v2u a, v2u b, v2u c, v4 colour)
{
    fill_triangle(image, V2S(a), V2S(b), V2S(c), colour);
}
#endif

internal inline void
fill_triangle(Image *image, v2 a, v2 b, v2 c, v4 colour)
{
    v2s au = V2S(round(a.x), round(a.y));
    v2s bu = V2S(round(b.x), round(b.y));
    v2s cu = V2S(round(c.x), round(c.y));
    fill_triangle(image, au, bu, cu, colour);
}

internal inline void
fill_triangle(Image *image, v2s a, v2s b, v2s c, u32 colour)
{
    fill_triangle(image, a, b, c, unpack_colour(colour));
}

internal void
fill_circle(Image *image, s32 xStart, s32 yStart, u32 radius, v4 colour)
{
    s32 size = 2 * radius;
    
    f32 r = (s32)radius;
    f32 maxDistSqr = r * r;
    
    xStart = xStart - (s32)radius + 1;
    yStart = yStart - (s32)radius + 1;
    
    for (s32 y = yStart; y < yStart + size; ++y)
    {
            f32 fY = (f32)(y - yStart) - r + 0.5f;
            f32 fYSqr = fY * fY;
            for (s32 x = xStart; x < xStart + size; ++x)
            {
                    f32 fX = (f32)(x - xStart) - r + 0.5f;
                    f32 distSqr = fX * fX + fYSqr;
                    if ((distSqr < maxDistSqr) &&
                (0 <= x) && (x < image->width) &&
                (0 <= y) && (y < image->height))
                    {
                        draw_pixel(image, x, y, colour);
                    }
                }
            }
        }
    
internal void
fill_circle(Image *image, s32 xStart, s32 yStart, u32 radius, u32 colour)
{
    fill_circle(image, xStart, yStart, radius, unpack_colour(colour));
}

//
// NOTE(michiel): Image drawing
//

internal inline void
draw_image(Image *screen, u32 xStart, u32 yStart, Image *image, v4 modColour = V4(1, 1, 1, 1))
{
    u32 *imageAt = image->pixels;
    for (u32 y = yStart; (y < (yStart + image->height)) && (y < screen->height); ++y)
    {
        for (u32 x = xStart; (x < (xStart + image->width)) && (x < screen->width); ++x)
        {
            v4 pixel = unpack_colour(*imageAt++);
            pixel.r *= modColour.r;
            pixel.g *= modColour.g;
            pixel.b *= modColour.b;
            pixel.a *= modColour.a;
            draw_pixel(screen, x, y, pixel);
        }
    }
}

//
// NOTE(michiel): Font drawing
//

internal inline void
draw_text(BitmapFont *font, Image *image, u32 xStart, u32 yStart, char *text,
          v4 colour = {1, 1, 1, 1})
{
    f32 x = xStart;
    f32 y = yStart + get_starting_baseline_y(&font->info);
    
    u32 prevPoint = 0;
    u8 *source = (u8 *)text;
    while (*source)
    {
        u32 codePoint = 0;
        u32 skip = get_code_point_from_utf8(source, &codePoint);
        if (!skip)
        {
            fprintf(stderr, "Malformed code point: 0x%X\n", codePoint);
            ++source;
            continue;
        }
        
        f32 advance = 0.0f;
        
        if (codePoint == (u32)'\n')
        {
            x = xStart;
            y += get_line_advance_for(&font->info);
        }
        else
        {
            advance = get_horizontal_advance_for_pair(font, prevPoint, codePoint);
            x += advance;
        }
        
        if (codePoint != ' ')
        {
            u32 glyphIndex = get_glyph_from_code_point(font, codePoint);
            
            if (glyphIndex)
            {
                FontGlyph *glyph = font->glyphs + glyphIndex;
                draw_image(image, x, y + glyph->yOffset, &glyph->bitmap, colour);
            }
        }
        
        prevPoint = codePoint;
        source += skip;
    }
}

internal inline void
draw_text(BitmapFont *font, Image *image, u32 xStart, u32 yStart, String text,
          v4 colour = {1, 1, 1, 1})
{
    draw_text(font, image, xStart, yStart, (char *)text.data, colour);
}
