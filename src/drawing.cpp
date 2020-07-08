#ifndef DRAWING_SLOW
#define DRAWING_SLOW 0
#endif

internal void fill_rectangle(Image *image, v2 start, v2 dim, v4 colour);

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
clear_region(Image *image, s32 startX, s32 startY, s32 width, s32 height)
{
    u32 endX = minimum(image->width, startX + width);
    u32 endY = minimum(image->height, startY + height);
    startX = maximum(0, startX);
    startY = maximum(0, startY);
    u32 *destRow = image->pixels + startY * image->rowStride;
    for (u32 y = startY; y < endY; ++y)
    {
        u32 *dest = destRow + startX;
        for (u32 x = startX; x < endX; ++x)
        {
            *dest++ = 0;
        }
        destRow += image->rowStride;
    }
}

internal void
clear(Image *image)
{
    clear_region(image, 0, 0, image->width, image->height);
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
#if 0
    result.rgb = src.rgb * (1.0f - overlay.a) + overlay.rgb;
    result.a = overlay.a;
#else
    result = overlay + (1.0f - overlay.a) * src;
#endif
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

    v4 source = unpack_colour(image->pixels[y * image->rowStride + x]);
    source = alpha_blend_colours(source, colour);
    image->pixels[y * image->rowStride + x] = pack_colour(source);
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
    v2 rectMin = V2(0, 0);
    v2 rectMax = V2((f32)image->width - 1, (f32)image->height - 1);

    f32 maxLinePoint = 1.0e20f;

    // NOTE(michiel): Remove nan/inf from the equation
    start.x = make_real(start.x, maxLinePoint);
    start.y = make_real(start.y, maxLinePoint);
    end.x = make_real(end.x, maxLinePoint);
    end.y = make_real(end.y, maxLinePoint);

    u32 startFlag = calculate_point_outside_rect(start, rectMin, rectMax);
    u32 endFlag   = calculate_point_outside_rect(end, rectMin, rectMax);

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
            start = calculate_intersection(start, end, rectMin, rectMax, startFlag);
            startFlag = calculate_point_outside_rect(start, rectMin, rectMax);
        }
        else
        {
            end = calculate_intersection(start, end, rectMin, rectMax, endFlag);
            endFlag = calculate_point_outside_rect(end, rectMin, rectMax);
        }
    }

    if (drawLine)
    {
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
                draw_pixel(image, xPixel1, yPixel1, pixel);

                if (yPixel1 + 1 < image->height)
                {
                    pixel.a = colour.a * fraction(yEnd) * xGap;
                    pixel.rgb = pixel.a * colour.rgb;
                    draw_pixel(image, xPixel1, yPixel1 + 1, pixel);
                }

                f32 intery = yEnd + gradient;

                xEnd = round(end.x);
                yEnd = end.y + gradient * (xEnd - end.x);
                xGap = fraction(end.x + 0.5f);

                s32 xPixel2 = (s32)xEnd;
                s32 yPixel2 = (s32)yEnd;
                pixel.a = colour.a * (1.0f - fraction(yEnd)) * xGap;
                pixel.rgb = pixel.a * colour.rgb;
                draw_pixel(image, xPixel2, yPixel2, pixel);

                if (yPixel2 + 1 < image->height)
                {
                    pixel.a = colour.a * fraction(yEnd) * xGap;
                    pixel.rgb = pixel.a * colour.rgb;
                    draw_pixel(image, xPixel2, yPixel2 + 1, pixel);
                }

                for (s32 x = xPixel1 + 1; x < xPixel2; ++x)
                {
                    pixel.a = colour.a * (1.0f - fraction(intery));
                    pixel.rgb = pixel.a * colour.rgb;
                    draw_pixel(image, x, (s32)intery, pixel);
                    pixel.a = colour.a * fraction(intery);
                    pixel.rgb = pixel.a * colour.rgb;
                    draw_pixel(image, x, (s32)intery + 1, pixel);
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
                draw_pixel(image, xPixel1, yPixel1, pixel);

                if (xPixel1 + 1 < image->width)
                {
                    pixel.a = colour.a * fraction(xEnd) * yGap;
                    pixel.rgb = pixel.a * colour.rgb;
                    draw_pixel(image, xPixel1 + 1, yPixel1, pixel);
                }

                f32 intery = xEnd + gradient;

                yEnd = round(end.y);
                xEnd = end.x + gradient * (yEnd - end.y);
                yGap = fraction(end.y + 0.5f);

                s32 xPixel2 = (s32)xEnd;
                s32 yPixel2 = (s32)yEnd;
                pixel.a = colour.a * (1.0f - fraction(xEnd)) * yGap;
                pixel.rgb = pixel.a * colour.rgb;
                draw_pixel(image, xPixel2, yPixel2, pixel);

                if (xPixel2 + 1 < image->width)
                {
                    pixel.a = colour.a * fraction(xEnd) * yGap;
                    pixel.rgb = pixel.a * colour.rgb;
                    draw_pixel(image, xPixel2 + 1, yPixel2, pixel);
                }

                for (s32 y = yPixel1 + 1; y < yPixel2; ++y)
                {
                    pixel.a = colour.a * (1.0f - fraction(intery));
                    pixel.rgb = pixel.a * colour.rgb;
                    draw_pixel(image, (s32)intery, y, pixel);
                    pixel.a = colour.a * fraction(intery);
                    pixel.rgb = pixel.a * colour.rgb;
                    draw_pixel(image, (s32)intery + 1, y, pixel);
                    intery += gradient;
                }
            }
        }
    }
}

#if 0
internal void
draw_line(Image *image, v2 start, v2 end, v4 colourStart, v4 colourEnd)
{
    // NOTE(michiel): Xiaolin Wu's line algorithm
    v2 rectMin = V2(0, 0);
    v2 rectMax = V2((f32)image->width - 1, (f32)image->height - 1);

    f32 maxLinePoint = 1.0e20f;
    if (is_neg_nan(start.x) || (start.x < -maxLinePoint))
    {
        start.x = -maxLinePoint;
    }
    if (is_neg_nan(start.y) || (start.y < -maxLinePoint))
    {
        start.y = -maxLinePoint;
    }
    if (is_pos_nan(end.x) || (end.x > maxLinePoint))
    {
        end.x = maxLinePoint;
    }
    if (is_pos_nan(end.y) || (end.y > maxLinePoint))
    {
        end.y = maxLinePoint;
    }

    u32 startFlag = calculate_point_outside_rect(start, rectMin, rectMax);
    u32 endFlag   = calculate_point_outside_rect(end, rectMin, rectMax);

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
            start = calculate_intersection(start, end, rectMin, rectMax, startFlag);
            startFlag = calculate_point_outside_rect(start, rectMin, rectMax);
        }
        else
        {
            end = calculate_intersection(start, end, rectMin, rectMax, endFlag);
            endFlag = calculate_point_outside_rect(end, rectMin, rectMax);
        }
    }

    if (drawLine)
    {
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
                    v4 tempCol = colourStart;
                    colourStart = colourEnd;
                    colourEnd = tempCol;
                }

                f32 gradient = diff.y / diff.x;
                f32 xEnd = round(start.x);
                f32 yEnd = start.y + gradient * (xEnd - start.x);
                f32 xGap = 1.0f - fraction(start.x + 0.5f);

                s32 xPixel1 = (s32)xEnd;
                s32 yPixel1 = (s32)yEnd;
                pixel.a = colourStart.a * (1.0f - fraction(yEnd)) * xGap;
                pixel.rgb = pixel.a * colourStart.rgb;
                draw_pixel(image, xPixel1, yPixel1, pixel);

                pixel.a = colourStart.a * fraction(yEnd) * xGap;
                pixel.rgb = pixel.a * colourStart.rgb;
                draw_pixel(image, xPixel1, yPixel1 + 1, pixel);

                f32 intery = yEnd + gradient;

                xEnd = round(end.x);
                yEnd = end.y + gradient * (xEnd - end.x);
                xGap = fraction(end.x + 0.5f);

                s32 xPixel2 = (s32)xEnd;
                s32 yPixel2 = (s32)yEnd;
                pixel.a = colourEnd.a * (1.0f - fraction(yEnd)) * xGap;
                pixel.rgb = pixel.a * colourEnd.rgb;
                draw_pixel(image, xPixel2, yPixel2, pixel);

                pixel.a = colourEnd.a * fraction(yEnd) * xGap;
                pixel.rgb = pixel.a * colourEnd.rgb;
                draw_pixel(image, xPixel2, yPixel2 + 1, pixel);

                for (s32 x = xPixel1 + 1; x < xPixel2; ++x)
                {
                    f32 t = (f32)(x - xPixel1 - 1) / (f32)(xPixel2 - xPixel1);
                    pixel.a = lerp(colourStart.a * (1.0f - fraction(intery)), t,
                                   colourEnd.a * (1.0f - fraction(intery)));
                    pixel.rgb = lerp(pixel.a * colourStart.rgb, t,
                                     pixel.a * colourEnd.rgb);

                    draw_pixel(image, x, (s32)intery, pixel);
                    pixel.a = lerp(colourStart.a * fraction(intery), t,
                                   colourEnd.a * fraction(intery));
                    pixel.rgb = lerp(pixel.a * colourStart.rgb, t,
                                     pixel.a * colourEnd.rgb);
                    draw_pixel(image, x, (s32)intery + 1, pixel);
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
                    v4 tempCol = colourStart;
                    colourStart = colourEnd;
                    colourEnd = tempCol;
                }

                f32 gradient = diff.x / diff.y;
                f32 yEnd = round(start.y);
                f32 xEnd = start.x + gradient * (yEnd - start.y);
                f32 yGap = 1.0f - fraction(start.y + 0.5f);

                s32 xPixel1 = (s32)xEnd;
                s32 yPixel1 = (s32)yEnd;
                pixel.a = colourStart.a * (1.0f - fraction(xEnd)) * yGap;
                pixel.rgb = pixel.a * colourStart.rgb;
                draw_pixel(image, xPixel1, yPixel1, pixel);

                pixel.a = colourStart.a * fraction(xEnd) * yGap;
                pixel.rgb = pixel.a * colourStart.rgb;
                draw_pixel(image, xPixel1 + 1, yPixel1, pixel);

                f32 intery = xEnd + gradient;

                yEnd = round(end.y);
                xEnd = end.x + gradient * (yEnd - end.y);
                yGap = fraction(end.y + 0.5f);

                s32 xPixel2 = (s32)xEnd;
                s32 yPixel2 = (s32)yEnd;
                pixel.a = colourEnd.a * (1.0f - fraction(xEnd)) * yGap;
                pixel.rgb = pixel.a * colourEnd.rgb;
                draw_pixel(image, xPixel2, yPixel2, pixel);

                pixel.a = colourEnd.a * fraction(xEnd) * yGap;
                pixel.rgb = pixel.a * colourEnd.rgb;
                draw_pixel(image, xPixel2 + 1, yPixel2, pixel);

                for (s32 y = yPixel1 + 1; y < yPixel2; ++y)
                {
                    f32 t = (f32)(y - yPixel1 - 1) / (f32)(yPixel2 - yPixel1);
                    pixel.a = lerp(colourStart.a * (1.0f - fraction(intery)), t,
                                   colourEnd.a * (1.0f - fraction(intery)));
                    pixel.rgb = lerp(pixel.a * colourStart.rgb, t,
                                     pixel.a * colourEnd.rgb);

                    draw_pixel(image, (s32)intery, y, pixel);
                    pixel.a = lerp(colourStart.a * fraction(intery), t,
                                   colourEnd.a * fraction(intery));
                    pixel.rgb = lerp(pixel.a * colourStart.rgb, t,
                                     pixel.a * colourEnd.rgb);
                    draw_pixel(image, (s32)intery + 1, y, pixel);
                    intery += gradient;
                }
            }
        }
    }
}
#else

#if 0
internal void
draw_line(Image *image, v2 start, v2 end, v4 colourStart, v4 colourEnd)
{
    // NOTE(michiel): Xiaolin Wu's line algorithm
    v2 rectMin = V2(0, 0);
    v2 rectMax = V2((f32)image->width - 1, (f32)image->height - 1);

    f32 maxLinePoint = 1.0e20f;
    if (is_neg_nan(start.x) || (start.x < -maxLinePoint))
    {
        start.x = -maxLinePoint;
    }
    if (is_neg_nan(start.y) || (start.y < -maxLinePoint))
    {
        start.y = -maxLinePoint;
    }
    if (is_pos_nan(end.x) || (end.x > maxLinePoint))
    {
        end.x = maxLinePoint;
    }
    if (is_pos_nan(end.y) || (end.y > maxLinePoint))
    {
        end.y = maxLinePoint;
    }

    u32 startFlag = calculate_point_outside_rect(start, rectMin, rectMax);
    u32 endFlag   = calculate_point_outside_rect(end, rectMin, rectMax);

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
            start = calculate_intersection(start, end, rectMin, rectMax, startFlag);
            startFlag = calculate_point_outside_rect(start, rectMin, rectMax);
        }
        else
        {
            end = calculate_intersection(start, end, rectMin, rectMax, endFlag);
            endFlag = calculate_point_outside_rect(end, rectMin, rectMax);
        }
    }

    if (drawLine)
    {
        v2 diff = end - start;
        v2 absDiff = absolute(diff);

        if (absDiff.x > absDiff.y)
        {
            if (absDiff.x > 0.0f)
            {
                if (end.x < start.x)
                {
                    v2 temp = start;
                    start = end;
                    end = temp;
                    v4 tempCol = colourStart;
                    colourStart = colourEnd;
                    colourEnd = tempCol;
                }

                v4 pixel0;
                v4 pixel1;
                v4 pixel2;
                v4 pixel3;

                f32 gradient = diff.y / diff.x;
                f32 xStart = round(start.x);
                f32 yStart = start.y + gradient * (xStart - start.x);
                f32 gapStart = 1.0f - fraction(start.x + 0.5f);

                f32 xEnd = round(end.x);
                f32 yEnd = end.y + gradient * (xEnd - end.x);
                f32 gapEnd = fraction(end.x + 0.5f);

                f32 intery = yStart + gradient;

                s32 xPixel1 = (s32)xStart;
                s32 yPixel1 = (s32)yStart;
                s32 xPixel2 = (s32)xEnd;
                s32 yPixel2 = (s32)yEnd;

                pixel0.a = colourStart.a * (1.0f - fraction(yStart)) * gapStart;
                pixel1.a = colourStart.a * fraction(yStart) * gapStart;
                pixel2.a = colourEnd.a * (1.0f - fraction(yEnd)) * gapEnd;
                pixel3.a = colourEnd.a * fraction(yEnd) * gapEnd;

                pixel0.rgb = pixel0.a * colourStart.rgb;
                pixel1.rgb = pixel1.a * colourStart.rgb;
                pixel2.rgb = pixel2.a * colourEnd.rgb;
                pixel3.rgb = pixel3.a * colourEnd.rgb;

                u32 *basePixels1 = image->pixels + yPixel1 * image->rowStride + xPixel1;
                u32 *basePixels2 = image->pixels + yPixel2 * image->rowStride + xPixel2;

                v4 source0 = unpack_colour(basePixels1[0]);
                v4 source1 = unpack_colour(basePixels1[image->rowStride]);
                v4 source2 = unpack_colour(basePixels2[0]);
                v4 source3 = unpack_colour(basePixels2[image->rowStride]);

                source0.rgb = source0.rgb * (1.0f - pixel0.a) + pixel0.rgb;
                source1.rgb = source1.rgb * (1.0f - pixel1.a) + pixel1.rgb;
                source2.rgb = source2.rgb * (1.0f - pixel2.a) + pixel2.rgb;
                source3.rgb = source3.rgb * (1.0f - pixel3.a) + pixel3.rgb;

                source0.a = pixel0.a;
                source1.a = pixel1.a;
                source2.a = pixel2.a;
                source3.a = pixel3.a;

                basePixels1[0] = pack_colour(source0);
                basePixels1[image->rowStride] = pack_colour(source1);
                basePixels2[0] = pack_colour(source2);
                basePixels2[image->rowStride] = pack_colour(source3);

                for (s32 x = xPixel1 + 1; x < xPixel2; x += 4)
                {
                    v4 pixelsA[4];
                    v4 pixelsB[4];

                    f32 intery0 = intery + 0.0f * gradient;
                    f32 intery1 = intery + 1.0f * gradient;
                    f32 intery2 = intery + 2.0f * gradient;
                    f32 intery3 = intery + 3.0f * gradient;
                    intery += 4.0f * gradient;

                    f32 t0 = (f32)(x - xPixel1 - 1) / (f32)(xPixel2 - xPixel1);
                    f32 t1 = (f32)(x - xPixel1 + 0) / (f32)(xPixel2 - xPixel1);
                    f32 t2 = (f32)(x - xPixel1 + 1) / (f32)(xPixel2 - xPixel1);
                    f32 t3 = (f32)(x - xPixel1 + 2) / (f32)(xPixel2 - xPixel1);

                    f32 fract0 = fraction(intery0);
                    f32 fract1 = fraction(intery1);
                    f32 fract2 = fraction(intery2);
                    f32 fract3 = fraction(intery3);
                    f32 oneMinF0 = 1.0f - fract0;
                    f32 oneMinF1 = 1.0f - fract1;
                    f32 oneMinF2 = 1.0f - fract2;
                    f32 oneMinF3 = 1.0f - fract3;

                    f32 lerpAlpha0 = lerp(colourStart.a, t0, colourEnd.a);
                    f32 lerpAlpha1 = lerp(colourStart.a, t1, colourEnd.a);
                    f32 lerpAlpha2 = lerp(colourStart.a, t2, colourEnd.a);
                    f32 lerpAlpha3 = lerp(colourStart.a, t3, colourEnd.a);

                    v3 lerpColour0 = lerp(colourStart.rgb, t0, colourEnd.rgb);
                    v3 lerpColour1 = lerp(colourStart.rgb, t1, colourEnd.rgb);
                    v3 lerpColour2 = lerp(colourStart.rgb, t2, colourEnd.rgb);
                    v3 lerpColour3 = lerp(colourStart.rgb, t3, colourEnd.rgb);

                    pixelsA[0].a = oneMinF0 * lerpAlpha0;
                    pixelsA[1].a = oneMinF1 * lerpAlpha1;
                    pixelsA[2].a = oneMinF2 * lerpAlpha2;
                    pixelsA[3].a = oneMinF3 * lerpAlpha3;

                    pixelsB[0].a = fract0 * lerpAlpha0;
                    pixelsB[1].a = fract1 * lerpAlpha1;
                    pixelsB[2].a = fract2 * lerpAlpha2;
                    pixelsB[3].a = fract3 * lerpAlpha3;

                    pixelsA[0].rgb = pixelsA[0].a * lerpColour0;
                    pixelsA[1].rgb = pixelsA[1].a * lerpColour1;
                    pixelsA[2].rgb = pixelsA[2].a * lerpColour2;
                    pixelsA[3].rgb = pixelsA[3].a * lerpColour3;

                    pixelsB[0].rgb = pixelsB[0].a * lerpColour0;
                    pixelsB[1].rgb = pixelsB[1].a * lerpColour1;
                    pixelsB[2].rgb = pixelsB[2].a * lerpColour2;
                    pixelsB[3].rgb = pixelsB[3].a * lerpColour3;

                    u32 offsetA0 = (s32)intery0 * image->rowStride + x + 0;
                    u32 offsetA1 = (s32)intery1 * image->rowStride + x + 1;
                    u32 offsetA2 = (s32)intery2 * image->rowStride + x + 2;
                    u32 offsetA3 = (s32)intery3 * image->rowStride + x + 3;
                    u32 offsetB0 = ((s32)intery0 + 1) * image->rowStride + x + 0;
                    u32 offsetB1 = ((s32)intery1 + 1) * image->rowStride + x + 1;
                    u32 offsetB2 = ((s32)intery2 + 1) * image->rowStride + x + 2;
                    u32 offsetB3 = ((s32)intery3 + 1) * image->rowStride + x + 3;
                    if ((x + 3) < xPixel2)
                    {
                        v4 sourceA0 = unpack_colour(image->pixels[offsetA0]);
                        v4 sourceA1 = unpack_colour(image->pixels[offsetA1]);
                        v4 sourceA2 = unpack_colour(image->pixels[offsetA2]);
                        v4 sourceA3 = unpack_colour(image->pixels[offsetA3]);

                        sourceA0.rgb = sourceA0.rgb * (1.0f - pixelsA[0].a) + pixelsA[0].rgb;
                        sourceA1.rgb = sourceA1.rgb * (1.0f - pixelsA[1].a) + pixelsA[1].rgb;
                        sourceA2.rgb = sourceA2.rgb * (1.0f - pixelsA[2].a) + pixelsA[2].rgb;
                        sourceA3.rgb = sourceA3.rgb * (1.0f - pixelsA[3].a) + pixelsA[3].rgb;

                        sourceA0.a = pixelsA[0].a;
                        sourceA1.a = pixelsA[1].a;
                        sourceA2.a = pixelsA[2].a;
                        sourceA3.a = pixelsA[3].a;

                        image->pixels[offsetA0] = pack_colour(sourceA0);
                        image->pixels[offsetA1] = pack_colour(sourceA1);
                        image->pixels[offsetA2] = pack_colour(sourceA2);
                        image->pixels[offsetA3] = pack_colour(sourceA3);

                        v4 sourceB0 = unpack_colour(image->pixels[offsetB0]);
                        v4 sourceB1 = unpack_colour(image->pixels[offsetB1]);
                        v4 sourceB2 = unpack_colour(image->pixels[offsetB2]);
                        v4 sourceB3 = unpack_colour(image->pixels[offsetB3]);

                        sourceB0.rgb = sourceB0.rgb * (1.0f - pixelsB[0].a) + pixelsB[0].rgb;
                        sourceB1.rgb = sourceB1.rgb * (1.0f - pixelsB[1].a) + pixelsB[1].rgb;
                        sourceB2.rgb = sourceB2.rgb * (1.0f - pixelsB[2].a) + pixelsB[2].rgb;
                        sourceB3.rgb = sourceB3.rgb * (1.0f - pixelsB[3].a) + pixelsB[3].rgb;

                        sourceB0.a = pixelsB[0].a;
                        sourceB1.a = pixelsB[1].a;
                        sourceB2.a = pixelsB[2].a;
                        sourceB3.a = pixelsB[3].a;

                        image->pixels[offsetB0] = pack_colour(sourceB0);
                        image->pixels[offsetB1] = pack_colour(sourceB1);
                        image->pixels[offsetB2] = pack_colour(sourceB2);
                        image->pixels[offsetB3] = pack_colour(sourceB3);
                    }
                    else if ((x + 2) < xPixel2)
                    {
                        v4 sourceA0 = unpack_colour(image->pixels[offsetA0]);
                        v4 sourceA1 = unpack_colour(image->pixels[offsetA1]);
                        v4 sourceA2 = unpack_colour(image->pixels[offsetA2]);

                        sourceA0.rgb = sourceA0.rgb * (1.0f - pixelsA[0].a) + pixelsA[0].rgb;
                        sourceA1.rgb = sourceA1.rgb * (1.0f - pixelsA[1].a) + pixelsA[1].rgb;
                        sourceA2.rgb = sourceA2.rgb * (1.0f - pixelsA[2].a) + pixelsA[2].rgb;

                        sourceA0.a = pixelsA[0].a;
                        sourceA1.a = pixelsA[1].a;
                        sourceA2.a = pixelsA[2].a;

                        image->pixels[offsetA0] = pack_colour(sourceA0);
                        image->pixels[offsetA1] = pack_colour(sourceA1);
                        image->pixels[offsetA2] = pack_colour(sourceA2);

                        v4 sourceB0 = unpack_colour(image->pixels[offsetB0]);
                        v4 sourceB1 = unpack_colour(image->pixels[offsetB1]);
                        v4 sourceB2 = unpack_colour(image->pixels[offsetB2]);

                        sourceB0.rgb = sourceB0.rgb * (1.0f - pixelsB[0].a) + pixelsB[0].rgb;
                        sourceB1.rgb = sourceB1.rgb * (1.0f - pixelsB[1].a) + pixelsB[1].rgb;
                        sourceB2.rgb = sourceB2.rgb * (1.0f - pixelsB[2].a) + pixelsB[2].rgb;

                        sourceB0.a = pixelsB[0].a;
                        sourceB1.a = pixelsB[1].a;
                        sourceB2.a = pixelsB[2].a;

                        image->pixels[offsetB0] = pack_colour(sourceB0);
                        image->pixels[offsetB1] = pack_colour(sourceB1);
                        image->pixels[offsetB2] = pack_colour(sourceB2);
                    }
                    else if ((x + 1) < xPixel2)
                    {
                        v4 sourceA0 = unpack_colour(image->pixels[offsetA0]);
                        v4 sourceA1 = unpack_colour(image->pixels[offsetA1]);

                        sourceA0.rgb = sourceA0.rgb * (1.0f - pixelsA[0].a) + pixelsA[0].rgb;
                        sourceA1.rgb = sourceA1.rgb * (1.0f - pixelsA[1].a) + pixelsA[1].rgb;

                        sourceA0.a = pixelsA[0].a;
                        sourceA1.a = pixelsA[1].a;

                        image->pixels[offsetA0] = pack_colour(sourceA0);
                        image->pixels[offsetA1] = pack_colour(sourceA1);

                        v4 sourceB0 = unpack_colour(image->pixels[offsetB0]);
                        v4 sourceB1 = unpack_colour(image->pixels[offsetB1]);

                        sourceB0.rgb = sourceB0.rgb * (1.0f - pixelsB[0].a) + pixelsB[0].rgb;
                        sourceB1.rgb = sourceB1.rgb * (1.0f - pixelsB[1].a) + pixelsB[1].rgb;

                        sourceB0.a = pixelsB[0].a;
                        sourceB1.a = pixelsB[1].a;

                        image->pixels[offsetB0] = pack_colour(sourceB0);
                        image->pixels[offsetB1] = pack_colour(sourceB1);
                    }
                    else
                    {
                        v4 sourceA0 = unpack_colour(image->pixels[offsetA0]);
                        v4 sourceB0 = unpack_colour(image->pixels[offsetB0]);

                        sourceA0.rgb = sourceA0.rgb * (1.0f - pixelsA[0].a) + pixelsA[0].rgb;
                        sourceB0.rgb = sourceB0.rgb * (1.0f - pixelsB[0].a) + pixelsB[0].rgb;

                        sourceA0.a = pixelsA[0].a;
                        sourceB0.a = pixelsB[0].a;

                        image->pixels[offsetA0] = pack_colour(sourceA0);
                        image->pixels[offsetB0] = pack_colour(sourceB0);
                    }
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
                    v4 tempCol = colourStart;
                    colourStart = colourEnd;
                    colourEnd = tempCol;
                }

                v4 pixels[4];

                f32 gradient = diff.x / diff.y;
                f32 yStart = round(start.y);
                f32 xStart = start.x + gradient * (yStart - start.y);
                f32 gapStart = 1.0f - fraction(start.y + 0.5f);

                f32 yEnd = round(end.y);
                f32 xEnd = end.x + gradient * (yEnd - end.y);
                f32 gapEnd = fraction(end.y + 0.5f);

                f32 intery = xStart + gradient;

                s32 xPixel1 = (s32)xStart;
                s32 yPixel1 = (s32)yStart;
                s32 xPixel2 = (s32)xEnd;
                s32 yPixel2 = (s32)yEnd;

                pixels[0].a = colourStart.a * (1.0f - fraction(xStart)) * gapStart;
                pixels[1].a = colourStart.a * fraction(xStart) * gapStart;
                pixels[2].a = colourEnd.a * (1.0f - fraction(xEnd)) * gapEnd;
                pixels[3].a = colourEnd.a * fraction(xEnd) * gapEnd;

                pixels[0].rgb = pixels[0].a * colourStart.rgb;
                pixels[1].rgb = pixels[1].a * colourStart.rgb;
                pixels[2].rgb = pixels[2].a * colourEnd.rgb;
                pixels[3].rgb = pixels[3].a * colourEnd.rgb;

                u32 *basePixels1 = image->pixels + yPixel1 * image->rowStride + xPixel1;
                u32 *basePixels2 = image->pixels + yPixel2 * image->rowStride + xPixel2;

                v4 source0 = unpack_colour(basePixels1[0]);
                v4 source1 = unpack_colour(basePixels1[1]);
                v4 source2 = unpack_colour(basePixels2[0]);
                v4 source3 = unpack_colour(basePixels2[1]);

                source0.rgb = source0.rgb * (1.0f - pixels[0].a) + pixels[0].rgb;
                source1.rgb = source1.rgb * (1.0f - pixels[1].a) + pixels[1].rgb;
                source2.rgb = source2.rgb * (1.0f - pixels[2].a) + pixels[2].rgb;
                source3.rgb = source3.rgb * (1.0f - pixels[3].a) + pixels[3].rgb;

                source0.a = pixels[0].a;
                source1.a = pixels[1].a;
                source2.a = pixels[2].a;
                source3.a = pixels[3].a;

                basePixels1[0] = pack_colour(source0);
                basePixels1[1] = pack_colour(source1);
                basePixels2[0] = pack_colour(source2);
                basePixels2[1] = pack_colour(source3);

                for (s32 y = yPixel1 + 1; y < yPixel2; y += 4)
                {
                    v4 pixelsA[4];
                    v4 pixelsB[4];

                    f32 intery0 = intery + 0.0f * gradient;
                    f32 intery1 = intery + 1.0f * gradient;
                    f32 intery2 = intery + 2.0f * gradient;
                    f32 intery3 = intery + 3.0f * gradient;
                    intery += 4.0f * gradient;

                    f32 t0 = (f32)(y - yPixel1 - 1) / (f32)(yPixel2 - yPixel1);
                    f32 t1 = (f32)(y - yPixel1 + 0) / (f32)(yPixel2 - yPixel1);
                    f32 t2 = (f32)(y - yPixel1 + 1) / (f32)(yPixel2 - yPixel1);
                    f32 t3 = (f32)(y - yPixel1 + 2) / (f32)(yPixel2 - yPixel1);

                    f32 fract0 = fraction(intery0);
                    f32 fract1 = fraction(intery1);
                    f32 fract2 = fraction(intery2);
                    f32 fract3 = fraction(intery3);
                    f32 oneMinF0 = 1.0f - fract0;
                    f32 oneMinF1 = 1.0f - fract1;
                    f32 oneMinF2 = 1.0f - fract2;
                    f32 oneMinF3 = 1.0f - fract3;

                    f32 lerpAlpha0 = lerp(colourStart.a, t0, colourEnd.a);
                    f32 lerpAlpha1 = lerp(colourStart.a, t1, colourEnd.a);
                    f32 lerpAlpha2 = lerp(colourStart.a, t2, colourEnd.a);
                    f32 lerpAlpha3 = lerp(colourStart.a, t3, colourEnd.a);

                    v3 lerpColour0 = lerp(colourStart.rgb, t0, colourEnd.rgb);
                    v3 lerpColour1 = lerp(colourStart.rgb, t1, colourEnd.rgb);
                    v3 lerpColour2 = lerp(colourStart.rgb, t2, colourEnd.rgb);
                    v3 lerpColour3 = lerp(colourStart.rgb, t3, colourEnd.rgb);

                    pixelsA[0].a = oneMinF0 * lerpAlpha0;
                    pixelsA[1].a = oneMinF1 * lerpAlpha1;
                    pixelsA[2].a = oneMinF2 * lerpAlpha2;
                    pixelsA[3].a = oneMinF3 * lerpAlpha3;

                    pixelsB[0].a = fract0 * lerpAlpha0;
                    pixelsB[1].a = fract1 * lerpAlpha1;
                    pixelsB[2].a = fract2 * lerpAlpha2;
                    pixelsB[3].a = fract3 * lerpAlpha3;

                    pixelsA[0].rgb = pixelsA[0].a * lerpColour0;
                    pixelsA[1].rgb = pixelsA[1].a * lerpColour1;
                    pixelsA[2].rgb = pixelsA[2].a * lerpColour2;
                    pixelsA[3].rgb = pixelsA[3].a * lerpColour3;

                    pixelsB[0].rgb = pixelsB[0].a * lerpColour0;
                    pixelsB[1].rgb = pixelsB[1].a * lerpColour1;
                    pixelsB[2].rgb = pixelsB[2].a * lerpColour2;
                    pixelsB[3].rgb = pixelsB[3].a * lerpColour3;

                    u32 offsetA0 = (y + 0) * image->rowStride + (s32)intery0;
                    u32 offsetB0 = (y + 0) * image->rowStride + (s32)intery0 + 1;
                    u32 offsetA1 = (y + 1) * image->rowStride + (s32)intery1;
                    u32 offsetB1 = (y + 1) * image->rowStride + (s32)intery1 + 1;
                    u32 offsetA2 = (y + 2) * image->rowStride + (s32)intery2;
                    u32 offsetB2 = (y + 2) * image->rowStride + (s32)intery2 + 1;
                    u32 offsetA3 = (y + 3) * image->rowStride + (s32)intery3;
                    u32 offsetB3 = (y + 3) * image->rowStride + (s32)intery3 + 1;
                    if ((y + 3) < yPixel2)
                    {
                        v4 sourceA0 = unpack_colour(image->pixels[offsetA0]);
                        v4 sourceB0 = unpack_colour(image->pixels[offsetB0]);
                        v4 sourceA1 = unpack_colour(image->pixels[offsetA1]);
                        v4 sourceB1 = unpack_colour(image->pixels[offsetB1]);
                        v4 sourceA2 = unpack_colour(image->pixels[offsetA2]);
                        v4 sourceB2 = unpack_colour(image->pixels[offsetB2]);
                        v4 sourceA3 = unpack_colour(image->pixels[offsetA3]);
                        v4 sourceB3 = unpack_colour(image->pixels[offsetB3]);

                        sourceA0.rgb = sourceA0.rgb * (1.0f - pixelsA[0].a) + pixelsA[0].rgb;
                        sourceA1.rgb = sourceA1.rgb * (1.0f - pixelsA[1].a) + pixelsA[1].rgb;
                        sourceA2.rgb = sourceA2.rgb * (1.0f - pixelsA[2].a) + pixelsA[2].rgb;
                        sourceA3.rgb = sourceA3.rgb * (1.0f - pixelsA[3].a) + pixelsA[3].rgb;

                        sourceA0.a = pixelsA[0].a;
                        sourceA1.a = pixelsA[1].a;
                        sourceA2.a = pixelsA[2].a;
                        sourceA3.a = pixelsA[3].a;

                        sourceB0.rgb = sourceB0.rgb * (1.0f - pixelsB[0].a) + pixelsB[0].rgb;
                        sourceB1.rgb = sourceB1.rgb * (1.0f - pixelsB[1].a) + pixelsB[1].rgb;
                        sourceB2.rgb = sourceB2.rgb * (1.0f - pixelsB[2].a) + pixelsB[2].rgb;
                        sourceB3.rgb = sourceB3.rgb * (1.0f - pixelsB[3].a) + pixelsB[3].rgb;

                        sourceB0.a = pixelsB[0].a;
                        sourceB1.a = pixelsB[1].a;
                        sourceB2.a = pixelsB[2].a;
                        sourceB3.a = pixelsB[3].a;

                        image->pixels[offsetA0] = pack_colour(sourceA0);
                        image->pixels[offsetB0] = pack_colour(sourceB0);
                        image->pixels[offsetA1] = pack_colour(sourceA1);
                        image->pixels[offsetB1] = pack_colour(sourceB1);
                        image->pixels[offsetA2] = pack_colour(sourceA2);
                        image->pixels[offsetB2] = pack_colour(sourceB2);
                        image->pixels[offsetA3] = pack_colour(sourceA3);
                        image->pixels[offsetB3] = pack_colour(sourceB3);
                    }
                    else if ((y + 2) < yPixel2)
                    {
                        v4 sourceA0 = unpack_colour(image->pixels[offsetA0]);
                        v4 sourceB0 = unpack_colour(image->pixels[offsetB0]);
                        v4 sourceA1 = unpack_colour(image->pixels[offsetA1]);
                        v4 sourceB1 = unpack_colour(image->pixels[offsetB1]);
                        v4 sourceA2 = unpack_colour(image->pixels[offsetA2]);
                        v4 sourceB2 = unpack_colour(image->pixels[offsetB2]);

                        sourceA0.rgb = sourceA0.rgb * (1.0f - pixelsA[0].a) + pixelsA[0].rgb;
                        sourceA1.rgb = sourceA1.rgb * (1.0f - pixelsA[1].a) + pixelsA[1].rgb;
                        sourceA2.rgb = sourceA2.rgb * (1.0f - pixelsA[2].a) + pixelsA[2].rgb;

                        sourceA0.a = pixelsA[0].a;
                        sourceA1.a = pixelsA[1].a;
                        sourceA2.a = pixelsA[2].a;

                        sourceB0.rgb = sourceB0.rgb * (1.0f - pixelsB[0].a) + pixelsB[0].rgb;
                        sourceB1.rgb = sourceB1.rgb * (1.0f - pixelsB[1].a) + pixelsB[1].rgb;
                        sourceB2.rgb = sourceB2.rgb * (1.0f - pixelsB[2].a) + pixelsB[2].rgb;

                        sourceB0.a = pixelsB[0].a;
                        sourceB1.a = pixelsB[1].a;
                        sourceB2.a = pixelsB[2].a;

                        image->pixels[offsetA0] = pack_colour(sourceA0);
                        image->pixels[offsetB0] = pack_colour(sourceB0);
                        image->pixels[offsetA1] = pack_colour(sourceA1);
                        image->pixels[offsetB1] = pack_colour(sourceB1);
                        image->pixels[offsetA2] = pack_colour(sourceA2);
                        image->pixels[offsetB2] = pack_colour(sourceB2);
                    }
                    else if ((y + 1) < yPixel2)
                    {
                        v4 sourceA0 = unpack_colour(image->pixels[offsetA0]);
                        v4 sourceB0 = unpack_colour(image->pixels[offsetB0]);
                        v4 sourceA1 = unpack_colour(image->pixels[offsetA1]);
                        v4 sourceB1 = unpack_colour(image->pixels[offsetB1]);

                        sourceA0.rgb = sourceA0.rgb * (1.0f - pixelsA[0].a) + pixelsA[0].rgb;
                        sourceA1.rgb = sourceA1.rgb * (1.0f - pixelsA[1].a) + pixelsA[1].rgb;

                        sourceA0.a = pixelsA[0].a;
                        sourceA1.a = pixelsA[1].a;

                        sourceB0.rgb = sourceB0.rgb * (1.0f - pixelsB[0].a) + pixelsB[0].rgb;
                        sourceB1.rgb = sourceB1.rgb * (1.0f - pixelsB[1].a) + pixelsB[1].rgb;

                        sourceB0.a = pixelsB[0].a;
                        sourceB1.a = pixelsB[1].a;

                        image->pixels[offsetA0] = pack_colour(sourceA0);
                        image->pixels[offsetB0] = pack_colour(sourceB0);
                        image->pixels[offsetA1] = pack_colour(sourceA1);
                        image->pixels[offsetB1] = pack_colour(sourceB1);
                    }
                    else
                    {
                        v4 sourceA0 = unpack_colour(image->pixels[offsetA0]);
                        v4 sourceB0 = unpack_colour(image->pixels[offsetB0]);

                        sourceA0.rgb = sourceA0.rgb * (1.0f - pixelsA[0].a) + pixelsA[0].rgb;
                        sourceB0.rgb = sourceB0.rgb * (1.0f - pixelsB[0].a) + pixelsB[0].rgb;

                        sourceA0.a = pixelsA[0].a;
                        sourceB0.a = pixelsB[0].a;

                        image->pixels[offsetA0] = pack_colour(sourceA0);
                        image->pixels[offsetB0] = pack_colour(sourceB0);
                    }
                }
            }
        }
    }
}
#else

internal void
draw_line(Image *image, v2 start, v2 end, v4 colourStart, v4 colourEnd)
{
    // NOTE(michiel): Xiaolin Wu's line algorithm
    v2 rectMin = V2(0, 0);
    v2 rectMax = V2((f32)image->width - 1, (f32)image->height - 1);

    f32 maxLinePoint = 1.0e20f;
    if (is_neg_nan(start.x) || (start.x < -maxLinePoint))
    {
        start.x = -maxLinePoint;
    }
    if (is_neg_nan(start.y) || (start.y < -maxLinePoint))
    {
        start.y = -maxLinePoint;
    }
    if (is_pos_nan(end.x) || (end.x > maxLinePoint))
    {
        end.x = maxLinePoint;
    }
    if (is_pos_nan(end.y) || (end.y > maxLinePoint))
    {
        end.y = maxLinePoint;
    }

    u32 startFlag = calculate_point_outside_rect(start, rectMin, rectMax);
    u32 endFlag   = calculate_point_outside_rect(end, rectMin, rectMax);

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
            start = calculate_intersection(start, end, rectMin, rectMax, startFlag);
            startFlag = calculate_point_outside_rect(start, rectMin, rectMax);
        }
        else
        {
            end = calculate_intersection(start, end, rectMin, rectMax, endFlag);
            endFlag = calculate_point_outside_rect(end, rectMin, rectMax);
        }
    }

    if (drawLine)
    {
        v2 diff = end - start;
        v2 absDiff = absolute(diff);

        if (absDiff.x > absDiff.y)
        {
            if (absDiff.x > 0.0f)
            {
                if (end.x < start.x)
                {
                    v2 temp = start;
                    start = end;
                    end = temp;
                    v4 tempCol = colourStart;
                    colourStart = colourEnd;
                    colourEnd = tempCol;
                }

                v4 pixel0;
                v4 pixel1;
                v4 pixel2;
                v4 pixel3;

                f32 gradient = diff.y / diff.x;
                f32 xStart = round(start.x);
                f32 yStart = start.y + gradient * (xStart - start.x);
                f32 gapStart = 1.0f - fraction(start.x + 0.5f);

                f32 xEnd = round(end.x);
                f32 yEnd = end.y + gradient * (xEnd - end.x);
                f32 gapEnd = fraction(end.x + 0.5f);

                f32 intery = yStart + gradient;

                s32 xPixel1 = (s32)xStart;
                s32 yPixel1 = (s32)yStart;
                s32 xPixel2 = (s32)xEnd;
                s32 yPixel2 = (s32)yEnd;

                pixel0.a = colourStart.a * (1.0f - fraction(yStart)) * gapStart;
                pixel1.a = colourStart.a * fraction(yStart) * gapStart;
                pixel2.a = colourEnd.a * (1.0f - fraction(yEnd)) * gapEnd;
                pixel3.a = colourEnd.a * fraction(yEnd) * gapEnd;

                pixel0.rgb = pixel0.a * colourStart.rgb;
                pixel1.rgb = pixel1.a * colourStart.rgb;
                pixel2.rgb = pixel2.a * colourEnd.rgb;
                pixel3.rgb = pixel3.a * colourEnd.rgb;

                u32 *basePixels1 = image->pixels + yPixel1 * image->rowStride + xPixel1;
                u32 *basePixels2 = image->pixels + yPixel2 * image->rowStride + xPixel2;

                v4 source0 = unpack_colour(basePixels1[0]);
                v4 source1 = unpack_colour(basePixels1[image->rowStride]);
                v4 source2 = unpack_colour(basePixels2[0]);
                v4 source3 = unpack_colour(basePixels2[image->rowStride]);

                source0.rgb = source0.rgb * (1.0f - pixel0.a) + pixel0.rgb;
                source1.rgb = source1.rgb * (1.0f - pixel1.a) + pixel1.rgb;
                source2.rgb = source2.rgb * (1.0f - pixel2.a) + pixel2.rgb;
                source3.rgb = source3.rgb * (1.0f - pixel3.a) + pixel3.rgb;

                source0.a = pixel0.a;
                source1.a = pixel1.a;
                source2.a = pixel2.a;
                source3.a = pixel3.a;

                basePixels1[0] = pack_colour(source0);
                basePixels1[image->rowStride] = pack_colour(source1);
                basePixels2[0] = pack_colour(source2);
                basePixels2[image->rowStride] = pack_colour(source3);

                f32_4x interies = F32_4x(intery, intery + 1.0f * gradient,
                                         intery + 2.0f * gradient, intery + 3.0f * gradient);
                f32_4x interyStep = F32_4x(4.0f * gradient);

                f32_4x oneOverCount = F32_4x(1.0f / (f32)(xPixel2 - xPixel1));
                f32_4x xMod = F32_4x(-1.0f, 0.0f, 1.0f, 2.0f);
                f32_4x xPixel1_4x = F32_4x((f32)xPixel1) + xMod;

                f32_4x const_0 = zero_f32_4x();
                f32_4x const_1 = F32_4x(1.0f);
                f32_4x const_255 = F32_4x(255.0f);
                f32_4x oneOver255 = F32_4x(1.0f / 255.0f);
                f32_4x pixelMask = S32_4x(0x00FF);
                f32_4x const_8_int = S32_4x(8);
                f32_4x const_16_int = S32_4x(16);
                f32_4x const_24_int = S32_4x(24);

                for (s32 x = xPixel1 + 1; x < xPixel2; x += 4)
                {
                    v4_4x pixelsA;
                    v4_4x pixelsB;

                    f32_4x t = F32_4x((f32)x) - xPixel1_4x;
                    t = t * oneOverCount;

                    f32_4x fract = fraction(interies);
                    f32_4x oneMinFract = const_1 - fract;

                    f32_4x lerpAlpha  = lerp(F32_4x(colourStart.a), t, F32_4x(colourEnd.a));
                    v3_4x  lerpColour = lerp(V3_4x(colourStart.rgb), t, V3_4x(colourEnd.rgb));

                    pixelsA.a = oneMinFract * lerpAlpha;
                    pixelsB.a = fract * lerpAlpha;
                    pixelsA.rgb = pixelsA.a * lerpColour;
                    pixelsB.rgb = pixelsB.a * lerpColour;

                    u32 offsetA0 = (s32)interies.e[0] * image->rowStride + x + 0;
                    u32 offsetA1 = (s32)interies.e[1] * image->rowStride + x + 1;
                    u32 offsetA2 = (s32)interies.e[2] * image->rowStride + x + 2;
                    u32 offsetA3 = (s32)interies.e[3] * image->rowStride + x + 3;
                    u32 offsetB0 = ((s32)interies.e[0] + 1) * image->rowStride + x + 0;
                    u32 offsetB1 = ((s32)interies.e[1] + 1) * image->rowStride + x + 1;
                    u32 offsetB2 = ((s32)interies.e[2] + 1) * image->rowStride + x + 2;
                    u32 offsetB3 = ((s32)interies.e[3] + 1) * image->rowStride + x + 3;

                    u32 pixelA0 = image->pixels[offsetA0];
                    u32 pixelA1 = 0;
                    u32 pixelA2 = 0;
                    u32 pixelA3 = 0;

                    u32 pixelB0 = image->pixels[offsetB0];
                    u32 pixelB1 = 0;
                    u32 pixelB2 = 0;
                    u32 pixelB3 = 0;
                    if ((x + 1) < xPixel2)
                    {
                        pixelA1 = image->pixels[offsetA1];
                        pixelB1 = image->pixels[offsetB1];
                        if ((x + 2) < xPixel2)
                        {
                            pixelA2 = image->pixels[offsetA2];
                            pixelB2 = image->pixels[offsetB2];
                            if ((x + 3) < xPixel2)
                            {
                                pixelA3 = image->pixels[offsetA3];
                                pixelB3 = image->pixels[offsetB3];
                            }
                        }
                    }

                    f32_4x sourceARaw = S32_4x(pixelA0, pixelA1, pixelA2, pixelA3);
                    f32_4x sourceBRaw = S32_4x(pixelB0, pixelB1, pixelB2, pixelB3);

                    v4_4x sourceA;
                    v4_4x sourceB;
                    sourceA.r = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceARaw, const_16_int), pixelMask));
                    sourceA.g = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceARaw, const_8_int), pixelMask));
                    sourceA.b = f32_4x_from_s32(s32_4x_and(sourceARaw, pixelMask));
                    sourceA.a = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceARaw, const_24_int), pixelMask));
                    sourceB.r = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceBRaw, const_16_int), pixelMask));
                    sourceB.g = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceBRaw, const_8_int), pixelMask));
                    sourceB.b = f32_4x_from_s32(s32_4x_and(sourceBRaw, pixelMask));
                    sourceB.a = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceBRaw, const_24_int), pixelMask));

                    sourceA = sourceA * oneOver255;
                    sourceB = sourceB * oneOver255;

                    sourceA.rgb = sourceA.rgb * (const_1 - pixelsA.a) + pixelsA.rgb;
                    sourceA.a = pixelsA.a;

                    sourceA = clamp_4x(const_0, sourceA, const_1);
                    sourceA = sourceA * const_255;

                    sourceB.rgb = sourceB.rgb * (const_1 - pixelsB.a) + pixelsB.rgb;
                    sourceB.a = pixelsB.a;

                    sourceB = clamp_4x(const_0, sourceB, const_1);
                    sourceB = sourceB * const_255;

                    image->pixels[offsetA0] = (((u32_from_f32_round(sourceA.a.e[0]) & 0xFF) << 24) |
                                               ((u32_from_f32_round(sourceA.r.e[0]) & 0xFF) << 16) |
                                               ((u32_from_f32_round(sourceA.g.e[0]) & 0xFF) <<  8) |
                                               ((u32_from_f32_round(sourceA.b.e[0]) & 0xFF) <<  0));

                    image->pixels[offsetB0] = (((u32_from_f32_round(sourceB.a.e[0]) & 0xFF) << 24) |
                                               ((u32_from_f32_round(sourceB.r.e[0]) & 0xFF) << 16) |
                                               ((u32_from_f32_round(sourceB.g.e[0]) & 0xFF) <<  8) |
                                               ((u32_from_f32_round(sourceB.b.e[0]) & 0xFF) <<  0));

                    if ((x + 1) < xPixel2)
                    {
                        image->pixels[offsetA1] = (((u32_from_f32_round(sourceA.a.e[1]) & 0xFF) << 24) |
                                                   ((u32_from_f32_round(sourceA.r.e[1]) & 0xFF) << 16) |
                                                   ((u32_from_f32_round(sourceA.g.e[1]) & 0xFF) <<  8) |
                                                   ((u32_from_f32_round(sourceA.b.e[1]) & 0xFF) <<  0));

                        image->pixels[offsetB1] = (((u32_from_f32_round(sourceB.a.e[1]) & 0xFF) << 24) |
                                                   ((u32_from_f32_round(sourceB.r.e[1]) & 0xFF) << 16) |
                                                   ((u32_from_f32_round(sourceB.g.e[1]) & 0xFF) <<  8) |
                                                   ((u32_from_f32_round(sourceB.b.e[1]) & 0xFF) <<  0));

                        if ((x + 2) < xPixel2)
                        {
                            image->pixels[offsetA2] = (((u32_from_f32_round(sourceA.a.e[2]) & 0xFF) << 24) |
                                                       ((u32_from_f32_round(sourceA.r.e[2]) & 0xFF) << 16) |
                                                       ((u32_from_f32_round(sourceA.g.e[2]) & 0xFF) <<  8) |
                                                       ((u32_from_f32_round(sourceA.b.e[2]) & 0xFF) <<  0));

                            image->pixels[offsetB2] = (((u32_from_f32_round(sourceB.a.e[2]) & 0xFF) << 24) |
                                                       ((u32_from_f32_round(sourceB.r.e[2]) & 0xFF) << 16) |
                                                       ((u32_from_f32_round(sourceB.g.e[2]) & 0xFF) <<  8) |
                                                       ((u32_from_f32_round(sourceB.b.e[2]) & 0xFF) <<  0));

                            if ((x + 3) < xPixel2)
                            {
                                image->pixels[offsetA3] = (((u32_from_f32_round(sourceA.a.e[3]) & 0xFF) << 24) |
                                                           ((u32_from_f32_round(sourceA.r.e[3]) & 0xFF) << 16) |
                                                           ((u32_from_f32_round(sourceA.g.e[3]) & 0xFF) <<  8) |
                                                           ((u32_from_f32_round(sourceA.b.e[3]) & 0xFF) <<  0));

                                image->pixels[offsetB3] = (((u32_from_f32_round(sourceB.a.e[3]) & 0xFF) << 24) |
                                                           ((u32_from_f32_round(sourceB.r.e[3]) & 0xFF) << 16) |
                                                           ((u32_from_f32_round(sourceB.g.e[3]) & 0xFF) <<  8) |
                                                           ((u32_from_f32_round(sourceB.b.e[3]) & 0xFF) <<  0));
                            }
                        }
                    }
                    interies = interies + interyStep;
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
                    v4 tempCol = colourStart;
                    colourStart = colourEnd;
                    colourEnd = tempCol;
                }

                v4 pixels[4];

                f32 gradient = diff.x / diff.y;
                f32 yStart = round(start.y);
                f32 xStart = start.x + gradient * (yStart - start.y);
                f32 gapStart = 1.0f - fraction(start.y + 0.5f);

                f32 yEnd = round(end.y);
                f32 xEnd = end.x + gradient * (yEnd - end.y);
                f32 gapEnd = fraction(end.y + 0.5f);

                f32 intery = xStart + gradient;

                s32 xPixel1 = (s32)xStart;
                s32 yPixel1 = (s32)yStart;
                s32 xPixel2 = (s32)xEnd;
                s32 yPixel2 = (s32)yEnd;

                pixels[0].a = colourStart.a * (1.0f - fraction(xStart)) * gapStart;
                pixels[1].a = colourStart.a * fraction(xStart) * gapStart;
                pixels[2].a = colourEnd.a * (1.0f - fraction(xEnd)) * gapEnd;
                pixels[3].a = colourEnd.a * fraction(xEnd) * gapEnd;

                pixels[0].rgb = pixels[0].a * colourStart.rgb;
                pixels[1].rgb = pixels[1].a * colourStart.rgb;
                pixels[2].rgb = pixels[2].a * colourEnd.rgb;
                pixels[3].rgb = pixels[3].a * colourEnd.rgb;

                u32 *basePixels1 = image->pixels + yPixel1 * image->rowStride + xPixel1;
                u32 *basePixels2 = image->pixels + yPixel2 * image->rowStride + xPixel2;

                v4 source0 = unpack_colour(basePixels1[0]);
                v4 source1 = unpack_colour(basePixels1[1]);
                v4 source2 = unpack_colour(basePixels2[0]);
                v4 source3 = unpack_colour(basePixels2[1]);

                source0.rgb = source0.rgb * (1.0f - pixels[0].a) + pixels[0].rgb;
                source1.rgb = source1.rgb * (1.0f - pixels[1].a) + pixels[1].rgb;
                source2.rgb = source2.rgb * (1.0f - pixels[2].a) + pixels[2].rgb;
                source3.rgb = source3.rgb * (1.0f - pixels[3].a) + pixels[3].rgb;

                source0.a = pixels[0].a;
                source1.a = pixels[1].a;
                source2.a = pixels[2].a;
                source3.a = pixels[3].a;

                basePixels1[0] = pack_colour(source0);
                basePixels1[1] = pack_colour(source1);
                basePixels2[0] = pack_colour(source2);
                basePixels2[1] = pack_colour(source3);

                f32_4x interies = F32_4x(intery, intery + 1.0f * gradient,
                                         intery + 2.0f * gradient, intery + 3.0f * gradient);
                f32_4x interyStep = F32_4x(4.0f * gradient);

                f32_4x oneOverCount = F32_4x(1.0f / (f32)(yPixel2 - yPixel1));
                f32_4x yMod = F32_4x(-1.0f, 0.0f, 1.0f, 2.0f);
                f32_4x yPixel1_4x = F32_4x((f32)yPixel1) + yMod;

                f32_4x const_0 = zero_f32_4x();
                f32_4x const_1 = F32_4x(1.0f);
                f32_4x const_255 = F32_4x(255.0f);
                f32_4x oneOver255 = F32_4x(1.0f / 255.0f);
                f32_4x pixelMask = S32_4x(0x00FF);
                f32_4x const_8_int = S32_4x(8);
                f32_4x const_16_int = S32_4x(16);
                f32_4x const_24_int = S32_4x(24);

                for (s32 y = yPixel1 + 1; y < yPixel2; y += 4)
                {
                    v4_4x pixelsA;
                    v4_4x pixelsB;

                    f32_4x t = F32_4x((f32)y) - yPixel1_4x;
                    t = t * oneOverCount;

                    f32_4x fract = fraction(interies);
                    f32_4x oneMinFract = const_1 - fract;

                    f32_4x lerpAlpha  = lerp(F32_4x(colourStart.a), t, F32_4x(colourEnd.a));
                    v3_4x  lerpColour = lerp(V3_4x(colourStart.rgb), t, V3_4x(colourEnd.rgb));

                    pixelsA.a = oneMinFract * lerpAlpha;
                    pixelsB.a = fract * lerpAlpha;
                    pixelsA.rgb = pixelsA.a * lerpColour;
                    pixelsB.rgb = pixelsB.a * lerpColour;

                    u32 offsetA0 = (y + 0) * image->rowStride + (s32)interies.e[0];
                    u32 offsetB0 = (y + 0) * image->rowStride + (s32)interies.e[0] + 1;
                    u32 offsetA1 = (y + 1) * image->rowStride + (s32)interies.e[1];
                    u32 offsetB1 = (y + 1) * image->rowStride + (s32)interies.e[1] + 1;
                    u32 offsetA2 = (y + 2) * image->rowStride + (s32)interies.e[2];
                    u32 offsetB2 = (y + 2) * image->rowStride + (s32)interies.e[2] + 1;
                    u32 offsetA3 = (y + 3) * image->rowStride + (s32)interies.e[3];
                    u32 offsetB3 = (y + 3) * image->rowStride + (s32)interies.e[3] + 1;

                    u32 pixelA0 = image->pixels[offsetA0];
                    u32 pixelA1 = 0;
                    u32 pixelA2 = 0;
                    u32 pixelA3 = 0;

                    u32 pixelB0 = image->pixels[offsetB0];
                    u32 pixelB1 = 0;
                    u32 pixelB2 = 0;
                    u32 pixelB3 = 0;

                    if ((y + 1) < yPixel2)
                    {
                        pixelA1 = image->pixels[offsetA1];
                        pixelB1 = image->pixels[offsetB1];
                        if ((y + 2) < yPixel2)
                        {
                            pixelA2 = image->pixels[offsetA2];
                            pixelB2 = image->pixels[offsetB2];
                            if ((y + 3) < yPixel2)
                            {
                                pixelA3 = image->pixels[offsetA3];
                                pixelB3 = image->pixels[offsetB3];
                            }
                        }
                    }

                    f32_4x sourceARaw = S32_4x(pixelA0, pixelA1, pixelA2, pixelA3);
                    f32_4x sourceBRaw = S32_4x(pixelB0, pixelB1, pixelB2, pixelB3);

                    v4_4x sourceA;
                    v4_4x sourceB;
                    sourceA.r = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceARaw, const_16_int), pixelMask));
                    sourceA.g = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceARaw, const_8_int), pixelMask));
                    sourceA.b = f32_4x_from_s32(s32_4x_and(sourceARaw, pixelMask));
                    sourceA.a = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceARaw, const_24_int), pixelMask));
                    sourceB.r = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceBRaw, const_16_int), pixelMask));
                    sourceB.g = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceBRaw, const_8_int), pixelMask));
                    sourceB.b = f32_4x_from_s32(s32_4x_and(sourceBRaw, pixelMask));
                    sourceB.a = f32_4x_from_s32(s32_4x_and(s32_4x_srl(sourceBRaw, const_24_int), pixelMask));

                    sourceA = sourceA * oneOver255;
                    sourceB = sourceB * oneOver255;

                    sourceA.rgb = sourceA.rgb * (const_1 - pixelsA.a) + pixelsA.rgb;
                    sourceA.a = pixelsA.a;

                    sourceA = clamp_4x(const_0, sourceA, const_1);
                    sourceA = sourceA * const_255;

                    sourceB.rgb = sourceB.rgb * (const_1 - pixelsB.a) + pixelsB.rgb;
                    sourceB.a = pixelsB.a;

                    sourceB = clamp_4x(const_0, sourceB, const_1);
                    sourceB = sourceB * const_255;

                    image->pixels[offsetA0] = (((u32_from_f32_round(sourceA.a.e[0]) & 0xFF) << 24) |
                                               ((u32_from_f32_round(sourceA.r.e[0]) & 0xFF) << 16) |
                                               ((u32_from_f32_round(sourceA.g.e[0]) & 0xFF) <<  8) |
                                               ((u32_from_f32_round(sourceA.b.e[0]) & 0xFF) <<  0));

                    image->pixels[offsetB0] = (((u32_from_f32_round(sourceB.a.e[0]) & 0xFF) << 24) |
                                               ((u32_from_f32_round(sourceB.r.e[0]) & 0xFF) << 16) |
                                               ((u32_from_f32_round(sourceB.g.e[0]) & 0xFF) <<  8) |
                                               ((u32_from_f32_round(sourceB.b.e[0]) & 0xFF) <<  0));

                    if ((y + 1) < yPixel2)
                    {
                        image->pixels[offsetA1] = (((u32_from_f32_round(sourceA.a.e[1]) & 0xFF) << 24) |
                                                   ((u32_from_f32_round(sourceA.r.e[1]) & 0xFF) << 16) |
                                                   ((u32_from_f32_round(sourceA.g.e[1]) & 0xFF) <<  8) |
                                                   ((u32_from_f32_round(sourceA.b.e[1]) & 0xFF) <<  0));

                        image->pixels[offsetB1] = (((u32_from_f32_round(sourceB.a.e[1]) & 0xFF) << 24) |
                                                   ((u32_from_f32_round(sourceB.r.e[1]) & 0xFF) << 16) |
                                                   ((u32_from_f32_round(sourceB.g.e[1]) & 0xFF) <<  8) |
                                                   ((u32_from_f32_round(sourceB.b.e[1]) & 0xFF) <<  0));
                        if ((y + 2) < yPixel2)
                        {
                            image->pixels[offsetA2] = (((u32_from_f32_round(sourceA.a.e[2]) & 0xFF) << 24) |
                                                       ((u32_from_f32_round(sourceA.r.e[2]) & 0xFF) << 16) |
                                                       ((u32_from_f32_round(sourceA.g.e[2]) & 0xFF) <<  8) |
                                                       ((u32_from_f32_round(sourceA.b.e[2]) & 0xFF) <<  0));

                            image->pixels[offsetB2] = (((u32_from_f32_round(sourceB.a.e[2]) & 0xFF) << 24) |
                                                       ((u32_from_f32_round(sourceB.r.e[2]) & 0xFF) << 16) |
                                                       ((u32_from_f32_round(sourceB.g.e[2]) & 0xFF) <<  8) |
                                                       ((u32_from_f32_round(sourceB.b.e[2]) & 0xFF) <<  0));
                            if ((y + 3) < yPixel2)
                            {
                                image->pixels[offsetA3] = (((u32_from_f32_round(sourceA.a.e[3]) & 0xFF) << 24) |
                                                           ((u32_from_f32_round(sourceA.r.e[3]) & 0xFF) << 16) |
                                                           ((u32_from_f32_round(sourceA.g.e[3]) & 0xFF) <<  8) |
                                                           ((u32_from_f32_round(sourceA.b.e[3]) & 0xFF) <<  0));

                                image->pixels[offsetB3] = (((u32_from_f32_round(sourceB.a.e[3]) & 0xFF) << 24) |
                                                           ((u32_from_f32_round(sourceB.r.e[3]) & 0xFF) << 16) |
                                                           ((u32_from_f32_round(sourceB.g.e[3]) & 0xFF) <<  8) |
                                                           ((u32_from_f32_round(sourceB.b.e[3]) & 0xFF) <<  0));
                            }
                        }
                    }
                    interies = interies + interyStep;
                }
            }
        }
    }
}

#endif


#endif

internal void
draw_line(Image *image, f32 startX, f32 startY, f32 endX, f32 endY, v4 colour = V4(1, 1, 1, 1))
{
    draw_line(image, V2(startX, startY), V2(endX, endY), colour);
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
    v2 prevP = points[0] + offset;
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        v2 P = points[pointIdx] + offset;
        draw_line(image, prevP, P, colour);
        prevP = P;
    }
}

internal void
draw_lines(Image *image, u32 pointCount, v2 *points, v2 offset, v2 scale = V2(1, 1),
           v4 colour = V4(1, 1, 1, 1))
{
    i_expect(pointCount);
    v2 prevP = hadamard(points[0], scale) + offset;
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        v2 P = hadamard(points[pointIdx], scale) + offset;
        draw_line(image, prevP, P, colour);
        prevP = P;
    }
}

internal void
draw_lines(Image *image, u32 pointCount, v2 *points, v4 *colours, v2 offset, v2 scale = V2(1, 1))
{
    i_expect(pointCount);
    v2 prevP = hadamard(points[0], scale) + offset;
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        v2 P = hadamard(points[pointIdx], scale) + offset;
        draw_line(image, prevP, P, colours[pointIdx - 1], colours[pointIdx]);
        prevP = P;
    }
}

struct ColouredPoint
{
    v2 point;
    v4 colour;
};
internal void
draw_lines(Image *image, u32 pointCount, ColouredPoint *points, v2 offset, v2 scale = V2(1, 1))
{
    i_expect(pointCount);
    ColouredPoint prevP = points[0];
    prevP.point.x *= scale.x;
    prevP.point.y *= scale.y;
    prevP.point += offset;
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        ColouredPoint P = points[pointIdx];
        P.point.x *= scale.x;
        P.point.y *= scale.y;
        P.point += offset;
        draw_line(image, prevP.point, P.point, prevP.colour, P.colour);
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

internal void
draw_lines(Image *image, u32 pointCount, f32 *xValues, f32 *yValues, v2 offset, v2 scale = V2(1, 1),
           v4 colour = V4(1, 1, 1, 1))
{
    i_expect(pointCount);
    v2 prevP = V2(xValues[0], yValues[0]);
    prevP = hadamard(prevP, scale) + offset;
    for (u32 pointIdx = 1; pointIdx < pointCount; ++pointIdx)
    {
        v2 P = hadamard(V2(xValues[pointIdx], yValues[pointIdx]), scale) + offset;
        draw_line(image, prevP, P, colour);
        prevP = P;
    }
}

internal void
draw_stems(Image *image, v2 pointSize, u32 pointCount, v2 *points, v2 offset, v2 scale = V2(1, 1),
           v4 colour = V4(1, 1, 1, 1))
{
    i_expect(pointCount);

    v2 pointOffset = 0.5f * (pointSize - V2(1, 1));

    for (u32 pointIdx = 0; pointIdx < pointCount; ++pointIdx)
    {
        v2 point = points[pointIdx];
        v2 P = hadamard(point, scale) + offset;
        v2 zeroPoint = hadamard(V2(point.x, 0), scale) + offset;
        draw_line(image, zeroPoint, P, colour);
        fill_rectangle(image, P - pointOffset, pointSize, colour);
    }
}

#if 0
internal void
draw_stems(Image *image, u32 pointCount, v3 *points, v3 offset, v3 scale = V3(1, 1, 1),
           v4 colourA = V4(1, 1, 1, 1), v4 colourB = V4(0.7f, 0.7f, 0.7f, 1))
{
    i_expect(pointCount);
    for (u32 pointIdx = 0; pointIdx < pointCount; ++pointIdx)
    {
        v3 zeroPoint = hadamard(V3(0, 0, 0), scale) + offset;
        v3 P = hadamard(points[pointIdx], scale) + offset;
        draw_line(image, zeroPoint.xy, P.xy, colourA);
        draw_line(image, V2(zeroPoint.x, zeroPoint.z), V2(P.x, P.z), colourB);
    }
}
#endif

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
outline_rectangle(Image *image, Rectangle2 rect, v4 colour)
{
    draw_line(image, rect.min, V2(rect.min.x, rect.max.y), colour);
    draw_line(image, V2(rect.min.x, rect.max.y), rect.max, colour);
    draw_line(image, rect.min, V2(rect.max.x, rect.min.y), colour);
    draw_line(image, V2(rect.max.x, rect.min.y), rect.max, colour);
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
    // NOTE(michiel): Non-optimized way to handle subpixels
    v2 endPos = pos + dim;

    v2 minP = floor(pos);
    v2 fullMaxP = floor(endPos);
    v2 fractMin = pos - minP;
    v2 fractMax = endPos - fullMaxP;
    v2 fullMinP = V2(fractMin.x ? minP.x + 1.0f : minP.x,
                     fractMin.y ? minP.y + 1.0f : minP.y);
    v2 maxP = V2(fractMax.x ? fullMaxP.x + 1.0f : fullMaxP.x,
                 fractMax.y ? fullMaxP.y + 1.0f : fullMaxP.y);

    u32 minDrawX = (u32)maximum(0, minP.x);
    u32 maxDrawX = (u32)minimum(image->width, maxP.x);
    u32 fullMinDrawX = (u32)maximum(0, fullMinP.x);
    u32 fullMaxDrawX = (u32)minimum(image->width, fullMaxP.x);

    f32 startXAlpha = 1.0f - fractMin.x;
    f32 endXAlpha = fractMax.x;

    u32 minDrawY = (u32)maximum(0, minP.y);
    u32 maxDrawY = (u32)minimum(image->height, maxP.y);
    u32 fullMinDrawY = (u32)maximum(0, fullMinP.y);
    u32 fullMaxDrawY = (u32)minimum(image->height, fullMaxP.y);

    f32 startYAlpha = 1.0f - fractMin.y;
    f32 endYAlpha = fractMax.y;

    // NOTE(michiel): First row
    if (minDrawY < fullMinDrawY)
    {
        for (u32 x = minDrawX; x < maxDrawX; ++x)
        {
            v4 p = colour;
            if (x < fullMinDrawX) {
                p.a *= startYAlpha * startXAlpha;
            } else if (x < fullMaxDrawX) {
                p.a *= startYAlpha;
            } else {
                p.a *= startYAlpha * endXAlpha;
            }
            p.rgb *= p.a;
            draw_pixel(image, x, minDrawY, p);
        }
    }

    // NOTE(michiel): Mid
    for (u32 y = fullMinDrawY; y < fullMaxDrawY; ++y)
    {
        for (u32 x = minDrawX; x < maxDrawX; ++x)
        {
            v4 p = colour;
            if (x < fullMinDrawX) {
                p.a *= startXAlpha;
            } else if (x >= fullMaxDrawX) {
                p.a *= endXAlpha;
            }
            p.rgb *= p.a;
            draw_pixel(image, x, y, p);
        }
    }

    // NOTE(michiel): End row
    if (fullMaxDrawY < maxDrawY)
    {
        for (u32 x = minDrawX; x < maxDrawX; ++x)
        {
            v4 p = colour;
            if (x < fullMinDrawX) {
                p.a *= endYAlpha * startXAlpha;
            } else if (x < fullMaxDrawX) {
                p.a *= endYAlpha;
            } else {
                p.a *= endYAlpha * endXAlpha;
            }
            p.rgb *= p.a;
            draw_pixel(image, x, fullMaxDrawY, p);
        }
    }
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

    u32 *dstRow = image->pixels + yStart * image->rowStride + xStart;
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
        dstRow += image->rowStride;
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

    s32 xMin = s32_from_f32_truncate(minX);
    s32 yMin = s32_from_f32_truncate(minY);
    s32 xMax = s32_from_f32_ceil(maxX);
    s32 yMax = s32_from_f32_ceil(maxY);

    xMin = clamp(0, xMin, (s32)image->width);
    yMin = clamp(0, yMin, (s32)image->height);
    xMax = clamp(0, xMax, (s32)image->width);
    yMax = clamp(0, yMax, (s32)image->height);

    for (s32 y = yMin; y < yMax; ++y)
    {
        for (s32 x = xMin; x < xMax; ++x)
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
    f32 diameter = 2.0f * radius;

    f32 maxDistSqr = square(radius - 0.5f);
    f32 edgeDistSqr = square(radius + 0.5f);

    f32 edgeDiff = 1.0f / (edgeDistSqr - maxDistSqr);

    s32 xMin = (s32)(x0 - radius);
    s32 yMin = (s32)(y0 - radius);
    s32 xMax = s32_from_f32_ceil(x0 + diameter);
    s32 yMax = s32_from_f32_ceil(y0 + diameter);

    xMin = clamp(0, xMin, (s32)image->width);
    yMin = clamp(0, yMin, (s32)image->height);
    xMax = clamp(0, xMax, (s32)image->width);
    yMax = clamp(0, yMax, (s32)image->height);

    for (s32 y = yMin; y < yMax; ++y)
    {
        f32 fY = (f32)y - y0;
        f32 fYSqr = square(fY);
        for (s32 x = xMin; x < xMax; ++x)
        {
            f32 fX = (f32)x - x0;
            f32 distSqr = square(fX) + fYSqr;

            if (distSqr <= edgeDistSqr)
            {
                v4 pixel = colour;

                if (distSqr > maxDistSqr)
                {
                    // adjust alpha for anti-aliasing
                    pixel.a -= (distSqr - maxDistSqr) * edgeDiff;
                    pixel.a = clamp01(pixel.a);
                }
                pixel.rgb *= pixel.a;

                draw_pixel(image, x, y, pixel);
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
fill_circle_gradient(Image *image, f32 x0, f32 y0, f32 radius, v4 colour = V4(1, 1, 1, 1), v4 edgeColour = V4(0, 0, 0, 1), f32 innerRadius = 0.0f)
{
    f32 diameter = 2.0f * radius;

    f32 maxDistSqr = square(radius - 0.5f);
    f32 edgeDistSqr = square(radius + 0.5f);
    f32 innerDistSqr = square(innerRadius);

    f32 edgeDiff = 1.0f / (edgeDistSqr - maxDistSqr);

    f32 edgeFactor = 1.0f / (maxDistSqr - innerDistSqr);

    s32 xMin = (s32)(x0 - radius);
    s32 yMin = (s32)(y0 - radius);
    s32 xMax = s32_from_f32_ceil(x0 + diameter);
    s32 yMax = s32_from_f32_ceil(y0 + diameter);

    xMin = clamp(0, xMin, (s32)image->width);
    yMin = clamp(0, yMin, (s32)image->height);
    xMax = clamp(0, xMax, (s32)image->width);
    yMax = clamp(0, yMax, (s32)image->height);

    for (s32 y = yMin; y < yMax; ++y)
    {
        f32 fY = (f32)y - y0;
        f32 fYSqr = square(fY);
        for (s32 x = xMin; x < xMax; ++x)
        {
            f32 fX = (f32)x - x0;
            f32 distSqr = square(fX) + fYSqr;

            if (distSqr <= innerDistSqr)
            {
                v4 pixel = colour;
                pixel.rgb *= pixel.a;
                draw_pixel(image, x, y, pixel);
            }
            else if (distSqr <= edgeDistSqr)
            {
                f32 colourFactor = clamp01((distSqr - innerDistSqr) * edgeFactor);
                v4 pixel = lerp(colour, colourFactor, edgeColour);

                if (distSqr > maxDistSqr)
                {
                    // adjust alpha for anti-aliasing
                    pixel.a -= (distSqr - maxDistSqr) * edgeDiff;
                    pixel.a = clamp01(pixel.a);
                }
                pixel.rgb *= pixel.a;

                draw_pixel(image, x, y, pixel);
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
    u32 xEnd = minimum(screen->width, xStart + image->width);
    u32 yEnd = minimum(screen->height, yStart + image->height);
    xStart = maximum(0, xStart);
    yStart = maximum(0, yStart);
    u32 *imageAt = image->pixels;
    for (u32 y = yStart; y < yEnd; ++y)
    {
        u32 *imageRow = imageAt;
        for (u32 x = xStart; x < xEnd; ++x)
        {
            v4 pixel = unpack_colour(*imageRow++);
            pixel = mix_colours(pixel, modColour);
            draw_pixel(screen, x, y, pixel);
        }
        imageAt += image->rowStride;
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
        imageAt += image->rowStride;
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
        imageAt += image->rowStride;
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
