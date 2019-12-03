#ifndef DRAWING_SLOW
#define DRAWING_SLOW 0
#endif

internal void
clear(Image *image)
{
    copy_single(image->width * image->height * sizeof(u32), 0, image->pixels);
}

internal void
clear_region(Image *image, s32 startX, s32 startY, s32 width, s32 height)
{
    startX = maximum(0, startX);
    startY = maximum(0, startY);
    u32 *destRow = image->pixels + startY * image->width;
    for (u32 y = 0; (y < height) && ((startY + y) < image->height); ++y)
    {
        u32 *dest = destRow + startX;
        for (u32 x = 0; (x < width) && ((startX + x) < image->width); ++x)
        {
            *dest++ = 0;
        }
        destRow += image->width;
    }
}

//
// NOTE(michiel): Colour mix helpers
//

internal v4
mix_colours(v4 src, v4 overlay)
{
    v4 result;
    result = hadamard(src, overlay);
    return result;
}

internal v4
alpha_blend_colours(v4 src, v4 overlay)
{
    v4 result;
    result.rgb = src.rgb * (1.0f - overlay.a) + overlay.rgb;
    result.a = overlay.a;
    return result;
}

//
// NOTE(michiel): Raw pixel setters
//

internal u32 *
draw_pixel(u32 *pixelAt, v4 colour)
{
    v4 source = unpack_colour(*pixelAt);
    source = alpha_blend_colours(source, colour);
    *pixelAt = pack_colour(source);
    return pixelAt + 1;
}

internal void
draw_pixel(Image *image, u32 x, u32 y, v4 colour)
{
#if DRAWING_SLOW
    i_expect(x < image->width);
    i_expect(y < image->height);
#endif // DRAWING_SLOW

    v4 source = unpack_colour(image->pixels[y * image->width + x]);
    source = alpha_blend_colours(source, colour);
    image->pixels[y * image->width + x] = pack_colour(source);
}

internal void
draw_pixel(Image *image, u32 x, u32 y, u32 colour)
{
    draw_pixel(image, x, y, unpack_colour(colour));
}

internal void
safe_draw_pixel(Image *image, s32 x, s32 y, v4 colour)
{
    if ((x >= 0) && (x < image->width) &&
        (y >= 0) && (y < image->height))
    {
        draw_pixel(image, x, y, colour);
    }
}

//
// NOTE(michiel): Lines
//

internal void
draw_line(Image *image, v2 start, v2 end, v4 colour = V4(1, 1, 1, 1))
{
    // NOTE(michiel): Xiaolin Wu's line algorithm

    // TODO(michiel): Border control (overflow/underflow drawing) instead of safe_draw_pixel
    v2 diff = end - start;
    v2 absDiff = absolute(diff);
    v4 pixel;

    if (absDiff.x > absDiff.y)
    {
        if (absDiff.x > 0.0f)
        {
            if (end.x < start.x)
            {
                v2 temp = start;
                start = end;
                end = temp;
            }

            f32 gradient = diff.y / diff.x;
            f32 xEnd = round(start.x);
            f32 yEnd = start.y + gradient * (xEnd - start.x);
            f32 xGap = 1.0f - fraction(start.x + 0.5f);

            s32 xPixel1 = (s32)xEnd;
            s32 yPixel1 = (s32)yEnd;
            pixel.a = colour.a * (1.0f - fraction(yEnd)) * xGap;
            pixel.rgb = pixel.a * colour.rgb;
            safe_draw_pixel(image, xPixel1, yPixel1, pixel);

            pixel.a = colour.a * fraction(yEnd) * xGap;
            pixel.rgb = pixel.a * colour.rgb;
            safe_draw_pixel(image, xPixel1, yPixel1 + 1, pixel);

            f32 intery = yEnd + gradient;

            xEnd = round(end.x);
            yEnd = end.y + gradient * (xEnd - end.x);
            xGap = fraction(end.x + 0.5f);

            s32 xPixel2 = (s32)xEnd;
            s32 yPixel2 = (s32)yEnd;
            pixel.a = colour.a * (1.0f - fraction(yEnd)) * xGap;
            pixel.rgb = pixel.a * colour.rgb;
            safe_draw_pixel(image, xPixel2, yPixel2, pixel);

            pixel.a = colour.a * fraction(yEnd) * xGap;
            pixel.rgb = pixel.a * colour.rgb;
            safe_draw_pixel(image, xPixel2, yPixel2 + 1, pixel);

            for (s32 x = xPixel1 + 1; x < xPixel2; ++x)
            {
                pixel.a = colour.a * (1.0f - fraction(intery));
                pixel.rgb = pixel.a * colour.rgb;
                safe_draw_pixel(image, x, (s32)intery, pixel);
                pixel.a = colour.a * fraction(intery);
                pixel.rgb = pixel.a * colour.rgb;
                safe_draw_pixel(image, x, (s32)intery + 1, pixel);
                intery += gradient;
            }
        }
    }
    else
    {
        if (absDiff.y > 0.0f)
        {
            if (end.y < start.y)
            {
                v2 temp = start;
                start = end;
                end = temp;
            }

            f32 gradient = diff.x / diff.y;
            f32 yEnd = round(start.y);
            f32 xEnd = start.x + gradient * (yEnd - start.y);
            f32 yGap = 1.0f - fraction(start.y + 0.5f);

            s32 xPixel1 = (s32)xEnd;
            s32 yPixel1 = (s32)yEnd;
            pixel.a = colour.a * (1.0f - fraction(xEnd)) * yGap;
            pixel.rgb = pixel.a * colour.rgb;
            safe_draw_pixel(image, xPixel1, yPixel1, pixel);

            pixel.a = colour.a * fraction(xEnd) * yGap;
            pixel.rgb = pixel.a * colour.rgb;
            safe_draw_pixel(image, xPixel1 + 1, yPixel1, pixel);

            f32 intery = xEnd + gradient;

            yEnd = round(end.y);
            xEnd = end.x + gradient * (yEnd - end.y);
            yGap = fraction(end.y + 0.5f);

            s32 xPixel2 = (s32)xEnd;
            s32 yPixel2 = (s32)yEnd;
            pixel.a = colour.a * (1.0f - fraction(xEnd)) * yGap;
            pixel.rgb = pixel.a * colour.rgb;
            safe_draw_pixel(image, xPixel2, yPixel2, pixel);

            pixel.a = colour.a * fraction(xEnd) * yGap;
            pixel.rgb = pixel.a * colour.rgb;
            safe_draw_pixel(image, xPixel2 + 1, yPixel2, pixel);

            for (s32 y = yPixel1 + 1; y < yPixel2; ++y)
            {
                pixel.a = colour.a * (1.0f - fraction(intery));
                pixel.rgb = pixel.a * colour.rgb;
                safe_draw_pixel(image, (s32)intery, y, pixel);
                pixel.a = colour.a * fraction(intery);
                pixel.rgb = pixel.a * colour.rgb;
                safe_draw_pixel(image, (s32)intery + 1, y, pixel);
                intery += gradient;
            }
        }
    }
}

internal void
draw_line(Image *image, s32 startX, s32 startY, s32 endX, s32 endY, v4 colour)
{
    draw_line(image, V2((f32)startX, (f32)startY), V2((f32)endX, (f32)endY), colour);
}

internal void
draw_line(Image *image, s32 startX, s32 startY, s32 endX, s32 endY, u32 colour)
{
    draw_line(image, startX, startY, endX, endY, unpack_colour(colour));
}

internal void
draw_lines(Image *image, u32 pointCount, v2 *points, v4 colour)
{
    i_expect(pointCount);
    v2 prevP = points[0];
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        v2 P = points[pointIdx];
        draw_line(image, prevP, P, colour);
        prevP = P;
    }
}

internal void
draw_lines(Image *image, u32 pointCount, v2 *points, v2 offset, v4 colour = V4(1, 1, 1, 1))
{
    i_expect(pointCount);
    v2 prevP = points[0];
    prevP += offset;
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        v2 P = points[pointIdx];
        P += offset;
        draw_line(image, prevP, P, colour);
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
        draw_line(image, prevP, P, colour);
        prevP = P;
    }
}

internal void
draw_lines(Image *image, u32 pointCount, v3 *points, v3 offset,
           v4 colourA = V4(1, 1, 1, 1), v4 colourB = V4(0.7f, 0.7f, 0.7f, 1))
{
    i_expect(pointCount);
    v3 prevP = points[0];
    prevP += offset;
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        v3 P = points[pointIdx];
        P += offset;
        draw_line(image, prevP.xy, P.xy, colourA);
        draw_line(image, V2(prevP.x, prevP.z), V2(P.x, P.z), colourB);
        prevP = P;
    }
}

internal void
draw_lines(Image *image, u32 pointCount, v3 *points, v3 offset, v3 scale = V3(1, 1, 1),
           v4 colourA = V4(1, 1, 1, 1), v4 colourB = V4(0.7f, 0.7f, 0.7f, 1))
{
    i_expect(pointCount);
    v3 prevP = points[0];
    prevP = hadamard(prevP, scale);
    prevP += offset;
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        v3 P = points[pointIdx];
        P = hadamard(P, scale);
        P += offset;
        draw_line(image, prevP.xy, P.xy, colourA);
        draw_line(image, V2(prevP.x, prevP.z), V2(P.x, P.z), colourB);
        prevP = P;
    }
}

//
// NOTE(michiel): Outlines
//

internal void
outline_rectangle(Image *image, u32 xStart, u32 yStart, u32 width, u32 height, v4 colour)
{
    colour.rgb *= colour.a;
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

internal void
outline_rectangle(Image *image, u32 xStart, u32 yStart, u32 width, u32 height, u32 colour)
{
    outline_rectangle(image, xStart, yStart, width, height, unpack_colour(colour));
}

internal void
outline_rectangle(Image *image, Rectangle2u rect, u32 colour)
{
    outline_rectangle(image, rect.min.x, rect.min.y, rect.max.x - rect.min.x, rect.max.y - rect.min.y, colour);
}

internal void
outline_triangle(Image *image, v2 a, v2 b, v2 c, v4 colour)
{
    draw_line(image, a.x, a.y, b.x, b.y, colour);
    draw_line(image, a.x, a.y, c.x, c.y, colour);
    draw_line(image, b.x, b.y, c.x, c.y, colour);
}

internal void
outline_triangle(Image *image, v2s a, v2s b, v2s c, v4 colour)
{
    outline_triangle(image, V2(a), V2(b), V2(c), colour);
}

internal void
outline_triangle(Image *image, v2u a, v2u b, v2u c, v4 colour)
{
    outline_triangle(image, V2(a), V2(b), V2(c), colour);
}

internal void
outline_triangle(Image *image, v2 a, v2 b, v2 c, u32 colour)
{
    outline_triangle(image, a, b, c, unpack_colour(colour));
}

internal void
outline_triangle(Image *image, v2s a, v2s b, v2s c, u32 colour)
{
    outline_triangle(image, a, b, c, unpack_colour(colour));
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
    colour.rgb *= colour.a;

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

internal void
fill_rectangle(Image *image, s32 xStart, s32 yStart, u32 width, u32 height, v4 colour)
{
    colour.rgb *= colour.a;

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

internal void
fill_rectangle(Image *image, u32 xStart, u32 yStart, u32 width, u32 height, u32 colour)
{
    fill_rectangle(image, xStart, yStart, width, height, unpack_colour(colour));
}

internal void
fill_rectangle(Image *image, v2 pos, v2 dim, v4 colour)
{
    fill_rectangle(image, s32_from_f32_round(pos.x), s32_from_f32_round(pos.y),
                   u32_from_f32_round(dim.x), u32_from_f32_round(dim.y), colour);
}

internal void
fill_rectangle(Image *image, v2 pos, v2 dim, u32 colour)
{
    fill_rectangle(image, pos, dim, unpack_colour(colour));
}

internal void
fill_tube(Image *image, u32 xStart, u32 yStart, u32 w, u32 h,
          v4 centerColour = V4(1, 1, 1, 1), v4 edgeColour = V4(0, 0, 0, 1))
{
    centerColour.rgb *= centerColour.a;
    edgeColour.rgb *= edgeColour.a;

    f32 radius = 0.5f * (f32)(h - 1);
    f32 maxDistSqr = square(radius);
    f32 edgeFactor = 1.0f / maxDistSqr;

    u32 *dstRow = image->pixels + yStart * image->width + xStart;
    for (s32 y = 0;
         (y < h) && ((y + yStart) < image->height);
         ++y)
    {
        u32 *dst = dstRow;
        f32 colourFactor = square((f32)y - radius) * edgeFactor;
        v4 pixel = lerp(centerColour, colourFactor, edgeColour);

        for (u32 x = 0; (x < w) && ((x + xStart) < image->width); ++x) {
            dst = draw_pixel(dst, pixel);
        }
        dstRow += image->width;
    }
}

internal f32
edge_function(v2 p, v2 lineStart, v2 lineEnd)
{
    // NOTE(michiel): Based on Juan Pineda's "A Parallel Algorithm for Polygon Rasterization"
    //   - returns > 0 when on the 'right' of the line
    //   - returns = 0 when on the line
    //   - returns < 0 when on the 'left' of the line
#if 0 // CLOCKWISE WINDING
    v2 line = lineEnd - lineStart;
    v2 point = p - lineStart;

    f32 result = point.x * line.y - point.y * line.x;
#else // COUNTER CLOCKWISE WINDING
    v2 line = lineStart - lineEnd;
    v2 point = p - lineStart;

    f32 result = line.x * point.y - line.y * point.x;
#endif
    return result;
}

internal v3
edge_offsets(v2 p, v2 a, v2 b, v2 c)
{
    v3 result;
    result.x = edge_function(p, a, b);
    result.y = edge_function(p, b, c);
    result.z = edge_function(p, c, a);
    return result;
}

internal b32
is_inside_triangle(v3 edgeOffsets)
{
    b32 result = true;
    result &= (edgeOffsets.x >= 0.0f);
    result &= (edgeOffsets.y >= 0.0f);
    result &= (edgeOffsets.z >= 0.0f);
    return result;
}

internal void
fill_triangle(Image *image, v2 a, v2 b, v2 c, v4 colour)
{
    colour.rgb *= colour.a;

    f32 minX = minimum(a.x, minimum(b.x, c.x));
    f32 maxX = maximum(a.x, maximum(b.x, c.x));
    f32 minY = minimum(a.y, minimum(b.y, c.y));
    f32 maxY = maximum(a.y, maximum(b.y, c.y));

    f32 modA = 1.0f / maximum(absolute(a.x - b.x), absolute(a.y - b.y));
    f32 modB = 1.0f / maximum(absolute(b.x - c.x), absolute(b.y - c.y));
    f32 modC = 1.0f / maximum(absolute(c.x - a.x), absolute(c.y - a.y));

    for (s32 y = s32_from_f32_truncate(minY); y < s32_from_f32_ceil(maxY); ++y)
    {
        for (s32 x = s32_from_f32_truncate(minX); x < s32_from_f32_ceil(maxX); ++x)
        {
            v2 point = V2(x, y);
            v3 edges = edge_offsets(point, a, b, c);

            f32 minP = edges.x;
            f32 modP = modA;
            if (minP > edges.y)
            {
                minP = edges.y;
                modP = modB;
            }
            if (minP > edges.z)
            {
                minP = edges.z;
                modP = modC;
            }

            f32 modAlpha = clamp01(1.0f + modP * minP);
            v4 pixel = colour;
            pixel *= modAlpha;

            draw_pixel(image, x, y, pixel);
        }
    }
}

internal void
fill_triangle(Image *image, v2s a, v2s b, v2s c, v4 colour)
{
    fill_triangle(image, V2(a), V2(b), V2(c), colour);
}

internal void
fill_triangle(Image *image, v2u a, v2u b, v2u c, v4 colour)
{
    fill_triangle(image, V2(a), V2(b), V2(c), colour);
}

internal void
fill_circle(Image *image, s32 xStart, s32 yStart, u32 radius, v4 colour)
{
    colour.rgb *= colour.a;

    s32 size = 2 * radius;

    f32 r = (f32)radius;
    f32 maxDistSqr = square(r);

    xStart = xStart - (s32)radius;
    yStart = yStart - (s32)radius;

    for (s32 y = yStart; y < yStart + size; ++y)
    {
        f32 fY = (f32)(y - yStart) - r;
        f32 fYSqr = fY * fY;
        for (s32 x = xStart; x < xStart + size; ++x)
        {
            f32 fX = (f32)(x - xStart) - r;
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

internal void
fill_circle(Image *image, f32 x0, f32 y0, f32 radius, v4 colour = V4(1, 1, 1, 1))
{
    radius -= 0.5f;
    f32 diameter = 2.0f * radius;

    f32 maxDistSqr = square(radius);
    f32 edgeDistSqr = square(radius + 1.0f);
    f32 edgeDiff = 1.0f / (edgeDistSqr - maxDistSqr);

    for (s32 y = 0, yOffset = (s32)(y0 - radius);
         (y < s32_from_f32_ceil(diameter + 1.0f)) &&
         (yOffset < image->width); ++y, ++yOffset)
    {
        f32 fY = (f32)y - radius - fraction(y0);
        f32 fYSqr = square(fY);
        for (s32 x = 0, xOffset = (s32)(x0 - radius);
             (x < s32_from_f32_ceil(diameter + 1.0f)) &&
             (xOffset < image->width); ++x, ++xOffset)
        {
            f32 fX = (f32)x - radius - fraction(x0);
            f32 distSqr = square(fX) + fYSqr;

            if (distSqr <= edgeDistSqr)
            {
                v4 pixel = colour;

                if (distSqr > maxDistSqr)
                {
                    // adjust alpha for anti-aliasing
                    pixel.a -= (distSqr - maxDistSqr) * edgeDiff;
                    clamp01(pixel.a);
                }
                pixel.rgb *= pixel.a;

                draw_pixel(image, x + (s32)(x0 - radius), y + (s32)(y0 - radius), pixel);
            }
        }
    }
}

internal void
fill_circle(Image *image, v2 pos, f32 radius, v4 colour = V4(1, 1, 1, 1))
{
    fill_circle(image, pos.x, pos.y, radius, colour);
}

internal void
fill_circle_gradient(Image *image, f32 x0, f32 y0, f32 radius, v4 colour = V4(1, 1, 1, 1), v4 edgeColour = V4(0, 0, 0, 1))
{
    radius -= 0.5f;
    f32 diameter = 2.0f * radius;

    f32 maxDistSqr = square(radius);
    f32 edgeDistSqr = square(radius + 1.0f);
    f32 edgeDiff = 1.0f / (edgeDistSqr - maxDistSqr);

    f32 edgeFactor = 1.0f / maxDistSqr;

    for (s32 y = 0, yOffset = (s32)(y0 - radius);
         (y < s32_from_f32_ceil(diameter + 1.0f)) &&
         (yOffset < image->width); ++y, ++yOffset)
    {
        f32 fY = (f32)y - radius - fraction(y0);
        f32 fYSqr = square(fY);
        for (s32 x = 0, xOffset = (s32)(x0 - radius);
             (x < s32_from_f32_ceil(diameter + 1.0f)) &&
             (xOffset < image->width); ++x, ++xOffset)
        {
            f32 fX = (f32)x - radius - fraction(x0);
            f32 distSqr = square(fX) + fYSqr;

            if (distSqr <= edgeDistSqr)
            {
                f32 colourFactor = clamp01(distSqr * edgeFactor);
                v4 pixel = lerp(colour, colourFactor, edgeColour);

                if (distSqr > maxDistSqr)
                {
                    // adjust alpha for anti-aliasing
                    pixel.a -= (distSqr - maxDistSqr) * edgeDiff;
                    clamp01(pixel.a);
                }
                pixel.rgb *= pixel.a;

                draw_pixel(image, x + (s32)(x0 - radius), y + (s32)(y0 - radius), pixel);
            }
        }
    }
}

//
// NOTE(michiel): Image drawing
//

internal void
draw_image(Image *screen, u32 xStart, u32 yStart, Image *image, v4 modColour = V4(1, 1, 1, 1))
{
    modColour.rgb *= modColour.a;
    u32 *imageAt = image->pixels;
    for (u32 y = yStart; (y < (yStart + image->height)) && (y < screen->height); ++y)
    {
        u32 *imageRow = imageAt;
        for (u32 x = xStart; (x < (xStart + image->width)) && (x < screen->width); ++x)
        {
            v4 pixel = unpack_colour(*imageRow++);
            pixel = mix_colours(pixel, modColour);
            draw_pixel(screen, x, y, pixel);
        }
        imageAt += image->width;
    }
}

internal void
draw_clipped_image(Image *screen, u32 xStart, u32 yStart, Image *image, Rectangle2u clipRect,
                   v4 modColour = V4(1, 1, 1, 1))
{
    modColour.rgb *= modColour.a;
    u32 *imageAt = image->pixels + clipRect.min.y * image->width + clipRect.min.x;
    v2u dimRect = get_dim(clipRect);
    for (u32 y = yStart;
         (y < (yStart + dimRect.height)) &&
         (y < (yStart + image->height)) &&
         (y < screen->height);
         ++y)
    {
        u32 *imageRow = imageAt;
        for (u32 x = xStart;
             (x < (xStart + dimRect.width)) &&
             (x < (xStart + image->width)) &&
             (x < screen->width);
             ++x)
        {
            v4 pixel = unpack_colour(*imageRow++);
            pixel = mix_colours(pixel, modColour);
            draw_pixel(screen, x, y, pixel);
        }
        imageAt += image->width;
    }
}

internal void
draw_image(Image *screen, u32 xStart, u32 yStart, Image8 *image, v4 modColour = V4(1, 1, 1, 1))
{
    modColour.rgb *= modColour.a;
    u8 *imageAt = image->pixels;
    for (u32 y = yStart; (y < (yStart + image->height)) && (y < screen->height); ++y)
    {
        u8 *imageRow = imageAt;
        for (u32 x = xStart; (x < (xStart + image->width)) && (x < screen->width); ++x)
        {
            v4 pixel = unpack_colour(*imageRow++);
            pixel = mix_colours(pixel, modColour);
            draw_pixel(screen, x, y, pixel);
        }
        imageAt += image->width;
    }
}

//
// NOTE(michiel): Font drawing
//

internal void
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
                draw_image(image, u32_from_f32_round(x), u32_from_f32_round(y + glyph->yOffset), &glyph->bitmap, colour);
            }
        }

        prevPoint = codePoint;
        source += skip;
    }
}

internal void
draw_text(BitmapFont *font, Image *image, u32 xStart, u32 yStart, String text,
          v4 colour = {1, 1, 1, 1})
{
    draw_text(font, image, xStart, yStart, (char *)text.data, colour);
}
