#ifndef DRAWING2D_SLOW
#define DRAWING2D_SLOW 0
#endif

// TODO(michiel): USE NEW IMAGE ROWSTRIDE!!!

struct Vertex2D
{
    v2 p;  // NOTE(michiel): Position
    v4 c;  // NOTE(michiel): Colour
};

//
// TODO(michiel): Move to some kind of line algorithm file (also in drawing_simd.cpp)
//
enum RectPositionFlag
{
    Outside_Inside = 0x00,
    Outside_Left   = 0x01,
    Outside_Right  = 0x02,
    Outside_Bottom = 0x04,
    Outside_Top    = 0x08,
};

internal u32
calculate_point_outside_rect(v2 point, v2 rectMin, v2 rectMax)
{
    u32 result = Outside_Inside;

    if (point.x < rectMin.x) { result |= Outside_Left; }
    if (point.y < rectMin.y) { result |= Outside_Bottom; }
    if (point.x > rectMax.x) { result |= Outside_Right; }
    if (point.y > rectMax.y) { result |= Outside_Top; }

    return result;
}

internal v2
calculate_intersection(v2 start, v2 end, v2 rectMin, v2 rectMax, u32 flags)
{
    v2 result = {};

    v2 delta = end - start;

    f32 slopeX = delta.y / delta.x;
    f32 slopeY = delta.x / delta.y;

    if (flags & Outside_Top)
    {
        result.x = start.x + slopeY * (rectMax.y - start.y);
        result.y = rectMax.y;
    }
    else if (flags & Outside_Bottom)
    {
        result.x = start.x + slopeY * (rectMin.y - start.y);
        result.y = rectMin.y;
    }
    else if (flags & Outside_Right)
    {
        result.x = rectMax.x;
        result.y = start.y + slopeX * (rectMax.x - start.x);
    }
    else if (flags & Outside_Left)
    {
        result.x = rectMin.x;
        result.y = start.y + slopeX * (rectMin.x - start.x);
    }
    else
    {
        INVALID_CODE_PATH;
    }

    return result;
}

//
//
//

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
    *pixelAt++ = pack_colour(source);
    return pixelAt;
}

internal void
draw_pixel(Image *image, u32 x, u32 y, v4 colour)
{
#if DRAWING2D_SLOW
    i_expect(x < image->width);
    i_expect(y < image->height);
#endif // DRAWING_SLOW

    v4 source = unpack_colour(image->pixels[y * image->width + x]);
    source = alpha_blend_colours(source, colour);
    image->pixels[y * image->width + x] = pack_colour(source);
}

//
// NOTE(michiel): Lines
//

internal void
draw_line(Image *image, Vertex2D start, Vertex2D end)
{
    // NOTE(michiel): Xiaolin Wu's line algorithm
    v2 rectMin = V2(0, 0);
    v2 rectMax = V2((f32)image->width - 1, (f32)image->height - 1);

    f32 maxLinePoint = 1.0e20f;
    if (is_neg_nan(start.p.x) || (start.p.x < -maxLinePoint))
    {
        start.p.x = -maxLinePoint;
    }
    if (is_neg_nan(start.p.y) || (start.p.y < -maxLinePoint))
    {
        start.p.y = -maxLinePoint;
    }
    if (is_pos_nan(end.p.x) || (end.p.x > maxLinePoint))
    {
        end.p.x = maxLinePoint;
    }
    if (is_pos_nan(end.p.y) || (end.p.y > maxLinePoint))
    {
        end.p.y = maxLinePoint;
    }

    u32 startFlag = calculate_point_outside_rect(start.p, rectMin, rectMax);
    u32 endFlag   = calculate_point_outside_rect(end.p, rectMin, rectMax);

    b32 drawLine = false;
    while (1)
    {
        if ((startFlag | endFlag) == Outside_Inside)
        {
            drawLine = true;
            break;
        }
        if ((startFlag & endFlag) != 0)
        {
            break;
        }

        if (startFlag != Outside_Inside)
        {
            start.p = calculate_intersection(start.p, end.p, rectMin, rectMax, startFlag);
            startFlag = calculate_point_outside_rect(start.p, rectMin, rectMax);
        }
        else
        {
            end.p = calculate_intersection(start.p, end.p, rectMin, rectMax, endFlag);
            endFlag = calculate_point_outside_rect(end.p, rectMin, rectMax);
        }
    }

    if (drawLine)
    {
        v2 diff = end.p - start.p;
        v2 absDiff = absolute(diff);

        v4 pixel;

        if (absDiff.x > absDiff.y)
        {
            if (absDiff.x > 0.0f)
            {
                if (end.p.x < start.p.x)
                {
                    Vertex2D temp = start;
                    start = end;
                    end = temp;
                }

                f32 gradient = diff.y / diff.x;
                f32 xEnd = round(start.p.x);
                f32 yEnd = start.p.y + gradient * (xEnd - start.p.x);
                f32 xGap = 1.0f - fraction(start.p.x + 0.5f);

                s32 xPixel1 = (s32)xEnd;
                s32 yPixel1 = (s32)yEnd;
                pixel.a = start.c.a * (1.0f - fraction(yEnd)) * xGap;
                pixel.rgb = pixel.a * start.c.rgb;
                draw_pixel(image, xPixel1, yPixel1, pixel);

                pixel.a = start.c.a * fraction(yEnd) * xGap;
                pixel.rgb = pixel.a * start.c.rgb;
                draw_pixel(image, xPixel1, yPixel1 + 1, pixel);

                f32 intery = yEnd + gradient;

                xEnd = round(end.p.x);
                yEnd = end.p.y + gradient * (xEnd - end.p.x);
                xGap = fraction(end.p.x + 0.5f);

                s32 xPixel2 = (s32)xEnd;
                s32 yPixel2 = (s32)yEnd;
                pixel.a = end.c.a * (1.0f - fraction(yEnd)) * xGap;
                pixel.rgb = pixel.a * end.c.rgb;
                draw_pixel(image, xPixel2, yPixel2, pixel);

                pixel.a = end.c.a * fraction(yEnd) * xGap;
                pixel.rgb = pixel.a * end.c.rgb;
                draw_pixel(image, xPixel2, yPixel2 + 1, pixel);

                for (s32 x = xPixel1 + 1; x < xPixel2; ++x)
                {
                    f32 t = (f32)(x - xPixel1 - 1) / (f32)(xPixel2 - xPixel1);
                    pixel.a = lerp(start.c.a * (1.0f - fraction(intery)), t,
                                   end.c.a * (1.0f - fraction(intery)));
                    pixel.rgb = lerp(pixel.a * start.c.rgb, t,
                                     pixel.a * end.c.rgb);

                    draw_pixel(image, x, (s32)intery, pixel);
                    pixel.a = lerp(start.c.a * fraction(intery), t,
                                   end.c.a * fraction(intery));
                    pixel.rgb = lerp(pixel.a * start.c.rgb, t,
                                     pixel.a * end.c.rgb);
                    draw_pixel(image, x, (s32)intery + 1, pixel);
                    intery += gradient;
                }
            }
        }
        else
        {
            if (absDiff.y > 0.0f)
            {
                if (end.p.y < start.p.y)
                {
                    Vertex2D temp = start;
                    start = end;
                    end = temp;
                }

                f32 gradient = diff.x / diff.y;
                f32 yEnd = round(start.p.y);
                f32 xEnd = start.p.x + gradient * (yEnd - start.p.y);
                f32 yGap = 1.0f - fraction(start.p.y + 0.5f);

                s32 xPixel1 = (s32)xEnd;
                s32 yPixel1 = (s32)yEnd;
                pixel.a = start.c.a * (1.0f - fraction(xEnd)) * yGap;
                pixel.rgb = pixel.a * start.c.rgb;
                draw_pixel(image, xPixel1, yPixel1, pixel);

                pixel.a = start.c.a * fraction(xEnd) * yGap;
                pixel.rgb = pixel.a * start.c.rgb;
                draw_pixel(image, xPixel1 + 1, yPixel1, pixel);

                f32 intery = xEnd + gradient;

                yEnd = round(end.p.y);
                xEnd = end.p.x + gradient * (yEnd - end.p.y);
                yGap = fraction(end.p.y + 0.5f);

                s32 xPixel2 = (s32)xEnd;
                s32 yPixel2 = (s32)yEnd;
                pixel.a = end.c.a * (1.0f - fraction(xEnd)) * yGap;
                pixel.rgb = pixel.a * end.c.rgb;
                draw_pixel(image, xPixel2, yPixel2, pixel);

                pixel.a = end.c.a * fraction(xEnd) * yGap;
                pixel.rgb = pixel.a * end.c.rgb;
                draw_pixel(image, xPixel2 + 1, yPixel2, pixel);

                for (s32 y = yPixel1 + 1; y < yPixel2; ++y)
                {
                    f32 t = (f32)(y - yPixel1 - 1) / (f32)(yPixel2 - yPixel1);
                    pixel.a = lerp(start.c.a * (1.0f - fraction(intery)), t,
                                   end.c.a * (1.0f - fraction(intery)));
                    pixel.rgb = lerp(pixel.a * start.c.rgb, t,
                                     pixel.a * end.c.rgb);

                    draw_pixel(image, (s32)intery, y, pixel);
                    pixel.a = lerp(start.c.a * fraction(intery), t,
                                   end.c.a * fraction(intery));
                    pixel.rgb = lerp(pixel.a * start.c.rgb, t,
                                     pixel.a * end.c.rgb);
                    draw_pixel(image, (s32)intery + 1, y, pixel);
                    intery += gradient;
                }
            }
        }
    }
}

internal void
draw_lines(Image *image, u32 pointCount, Vertex2D *points, v2 offset = V2(0, 0), v2 scale = V2(1, 1))
{
    i_expect(pointCount);
    Vertex2D prev = points[0];
    prev.p = hadamard(prev.p, scale);
    prev.p += offset;
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        Vertex2D v = points[pointIdx];
        v.p = hadamard(v.p, scale);
        v.p += offset;
        draw_line(image, prev, v);
        prev = v;
    }
}

//
// NOTE(michiel): Triangles and circles as filled primitives
//

#if 0
internal v4
barycentric_edgeoffsets(v2 p, v2 a, v2 b, v2 c,
                        f32 normalizeFactor, f32 modA, f32 modB, f32 modC)
{
    // NOTE(michiel): Barycentric coordinates in xyz and alpha in w (or a)
    // This function will discard clockwise winding triangles
    // TODO(michiel): Move to drawing3d where it should matter
    v4 result;

    f32 cXsubbX = c.x - b.x;
    f32 bYsubcY = b.y - c.y;
    f32 aXsubcX = a.x - c.x;
    //f32 aYsubcY = a.y - c.y; // == -(c.y - a.y)
    f32 cYsubaY = c.y - a.y;
    f32 pXsubcX = p.x - c.x;
    f32 pYsubcY = p.y - c.y;

    f32 mulcapx = cYsubaY * pXsubcX;
    f32 mulacpy = aXsubcX * pYsubcY;

    result.x = (bYsubcY * pXsubcX + cXsubbX * pYsubcY) * normalizeFactor;
    result.y = (mulcapx + mulacpy) * normalizeFactor;
    result.z = 1.0f - result.x - result.y;

    f32 edgeX = (a.x - b.x) * (p.y - a.y) - (a.y - b.y) * (p.x - a.x);
    f32 edgeY = -cXsubbX * (p.y - b.y) - bYsubcY * (p.x - b.x);
    f32 edgeZ = -mulacpy - mulcapx;

    f32 minP = edgeX;
    f32 modP = modA;
    if (minP > edgeY)
    {
        minP = edgeY;
        modP = modB;
    }
    if (minP > edgeZ)
    {
        minP = edgeZ;
        modP = modC;
    }
    result.a = clamp01(1.0f + minP / modP);

    return result;
}
#endif

internal v4
barycentric_alpha(v2 p, v2 a, v2 b, v2 c,
                  f32 normalizeFactor, f32 modA, f32 modB, f32 modC)
{
    // NOTE(michiel): Barycentric coordinates in xyz and alpha in w (or a)
    // For 2D we always draw triangles no matter the winding.
    v4 result;

    f32 cXsubbX = c.x - b.x;
    f32 bYsubcY = b.y - c.y;
    f32 aXsubcX = a.x - c.x;
    //f32 aYsubcY = a.y - c.y; // == -(c.y - a.y)
    f32 cYsubaY = c.y - a.y;
    f32 pXsubcX = p.x - c.x;
    f32 pYsubcY = p.y - c.y;

    f32 mulcapx = cYsubaY * pXsubcX;
    f32 mulacpy = aXsubcX * pYsubcY;

    result.x = (bYsubcY * pXsubcX + cXsubbX * pYsubcY) * normalizeFactor;
    result.y = (mulcapx + mulacpy) * normalizeFactor;
    result.z = 1.0f - result.x - result.y;

    f32 minP = result.x;
    f32 modP = modA;
    if (minP > result.y)
    {
        minP = result.y;
        modP = modB;
    }
    if (minP > result.z)
    {
        minP = result.z;
        modP = modC;
    }
    result.a = clamp01(1.0f + modP * minP);

    return result;
}

internal void
draw_triangle(Image *image, Vertex2D a, Vertex2D b, Vertex2D c)
{
    f32 minX = minimum3(a.p.x, b.p.x, c.p.x);
    f32 maxX = maximum3(a.p.x, b.p.x, c.p.x);
    f32 minY = minimum3(a.p.y, b.p.y, c.p.y);
    f32 maxY = maximum3(a.p.y, b.p.y, c.p.y);

    f32 modA = maximum(absolute(a.p.x - b.p.x), absolute(a.p.y - b.p.y));
    f32 modB = maximum(absolute(b.p.x - c.p.x), absolute(b.p.y - c.p.y));
    f32 modC = maximum(absolute(c.p.x - a.p.x), absolute(c.p.y - a.p.y));
    f32 normalizer = 1.0f / ((b.p.y - c.p.y) * (a.p.x - c.p.x) + (c.p.x - b.p.x) * (a.p.y - c.p.y));

    s32 xMin = s32_from_f32_truncate(minX);
    s32 yMin = s32_from_f32_truncate(minY);
    s32 xMax = s32_from_f32_ceil(maxX);
    s32 yMax = s32_from_f32_ceil(maxY);

    xMin = clamp(0, xMin, (s32)image->width);
    yMin = clamp(0, yMin, (s32)image->height);
    xMax = clamp(0, xMax, (s32)image->width);
    yMax = clamp(0, yMax, (s32)image->height);

    u32 *pixelRow = image->pixels + yMin * image->width + xMin;
    for (s32 y = yMin; y < yMax; ++y)
    {
        u32 *pixelAt = pixelRow;
        for (s32 x = xMin; x < xMax; ++x)
        {
            v2 point = V2(x, y);
            v4 baryEdge = barycentric_alpha(point, a, b, c, normalizer, modA, modB, modC);
            v4 pixel = baryEdge.x * a.c + baryEdge.y * b.c + baryEdge.z * c.c;
            pixel.a *= baryEdge.a;
            pixel.rgb *= pixel.a;
            pixel = clamp01(pixel);
            //draw_pixel(image, x, y, pixel);
            pixelAt = draw_pixel(pixelAt, pixel);
        }
        pixelRow += image->width;
    }
}

internal void
draw_circle(Image *image, Vertex2D center, Vertex2D edge)
{
    // NOTE(michiel):
    f32 radius = edge.p.x;
    f32 innerRadius = edge.p.y;

    f32 diameter = 2.0f * radius;

    f32 maxDistSqr = square(radius - 0.5f);
    f32 edgeDistSqr = square(radius + 0.5f);
    f32 innerDistSqr = square(innerRadius);

    f32 edgeDiff = 1.0f / (edgeDistSqr - maxDistSqr);

    f32 edgeFactor = 1.0f / (maxDistSqr - innerDistSqr);

    s32 xMin = (s32)(center.p.x - radius);
    s32 yMin = (s32)(center.p.y - radius);
    s32 xMax = s32_from_f32_ceil(center.p.x + diameter);
    s32 yMax = s32_from_f32_ceil(center.p.y + diameter);

    xMin = clamp(0, xMin, (s32)image->width);
    yMin = clamp(0, yMin, (s32)image->height);
    xMax = clamp(0, xMax, (s32)image->width);
    yMax = clamp(0, yMax, (s32)image->height);

    u32 *pixelRow = image->pixels + yMin * image->width + xMin;
    for (s32 y = yMin; y < yMax; ++y)
    {
        u32 *pixelAt = pixelRow;

        f32 fY = (f32)y - center.p.y;
        f32 fYSqr = square(fY);

        for (s32 x = xMin; x < xMax; ++x)
        {
            f32 fX = (f32)x - center.p.x;
            f32 distSqr = square(fX) + fYSqr;

            if (distSqr <= innerDistSqr)
            {
                v4 pixel = center.c;
                pixel.rgb *= pixel.a;
                //draw_pixel(image, x, y, pixel);
                pixelAt = draw_pixel(pixelAt, pixel);
            }
            else if (distSqr <= edgeDistSqr)
            {
                f32 colourFactor = clamp01((distSqr - innerDistSqr) * edgeFactor);
                v4 pixel = lerp(center.c, colourFactor, edge.c);

                if (distSqr > maxDistSqr)
                {
                    // adjust alpha for anti-aliasing
                    pixel.a -= (distSqr - maxDistSqr) * edgeDiff;
                    pixel.a = clamp01(pixel.a);
                }
                pixel.rgb *= pixel.a;

                //draw_pixel(image, x, y, pixel);
                pixelAt = draw_pixel(pixelAt, pixel);
            }
            else
            {
                ++pixelAt;
            }
        }
        pixelRow += image->width;
    }
}

//
// NOTE(michiel): Image drawing
//

internal void
draw_image(Image *screen, Vertex2D offset, Image *image)
{
    v4 modColour = offset.c;
    modColour.rgb *= modColour.a;

    s32 xMin = (s32)offset.p.x;
    s32 yMin = (s32)offset.p.y;
    s32 xMax = xMin + (s32)image->width;
    s32 yMax = yMin + (s32)image->height;

    xMin = clamp(0, xMin, (s32)screen->width);
    yMin = clamp(0, yMin, (s32)screen->height);
    xMax = clamp(0, xMax, (s32)screen->width);
    yMax = clamp(0, yMax, (s32)screen->height);

    // TODO(michiel): Use the fraction of the offset
    u32 *pixelRow = image->pixels;
    for (s32 y = yMin; y < yMax; ++y)
    {
        u32 *pixelAt = pixelRow;
        for (u32 x = xMin; x < xMax; ++x)
        {
            v4 pixel = unpack_colour(*pixelAt++);
            pixel = mix_colours(pixel, modColour);
            draw_pixel(screen, x, y, pixel);
        }
        pixelRow += image->width;
    }
}

#if 0
internal void
draw_image(Image *screen, Vertex2D offset, Image *image)
{
    v4 modColour = offset.c;
    modColour.rgb *= modColour.a;

    s32 xMin = (s32)offset.p.x;
    s32 yMin = (s32)offset.p.y;
    s32 xMax = s32_from_f32_ceil(offset.p.x + (f32)image->width);
    s32 yMax = s32_from_f32_ceil(offset.p.y + (f32)image->height);

    xMin = clamp(0, xMin, (s32)screen->width);
    yMin = clamp(0, yMin, (s32)screen->height);
    xMax = clamp(0, xMax, (s32)screen->width);
    yMax = clamp(0, yMax, (s32)screen->height);

    f32 fractionX = fraction(offset.p.x);
    f32 fractionY = fraction(offset.p.y);
    u32 *pixelRow0 = image->pixels;
    u32 *pixelRow1 = image->pixels + image->width;

    for (s32 y = yMin; y < yMax; ++y)
    {
        u32 *pixelAt0 = pixelRow0;
        u32 *pixelAt1 = pixelRow1;
        v4 prevX0 = unpack_colour(*pixelAt0++);
        v4 prevX1 = unpack_colour(*pixelAt1++);
        for (u32 x = xMin + 1; x < xMax; ++x)
        {
            v4 pixel0 = unpack_colour(*pixelAt0++);
            pixel0 = mix_colours(pixel0, modColour);
            v4 pixel1 = unpack_colour(*pixelAt1++);
            pixel1 = mix_colours(pixel1, modColour);
            v4 pixelx0 = lerp(prevX0, fractionX, pixel0);
            v4 pixelx1 = lerp(prevX1, fractionX, pixel1);
            v4 pixel = lerp(pixelx0, fractionY, pixelx1);
            draw_pixel(screen, x, yMin, pixel);
            prevX0 = pixel0;
            prevX1 = pixel1;
        }
        pixelRow0 += image->width;
        pixelRow1 += image->width;
    }
}
#endif

#if 0
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
#endif
