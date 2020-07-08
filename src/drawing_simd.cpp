// TODO(michiel): TEMP!!!!
//#include "platform.h"
//#include "multilane.h"

// TODO(michiel): USE NEW IMAGE ROWSTRIDE!!!
#if 0
//
// TODO(michiel): Move to some kind of line algorithm file (also in drawing.cpp)
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

internal void
clear(Image *image)
{
    copy_single(image->width * image->height * sizeof(u32), 0, image->pixels);
}
#endif

internal void
clear_region_simd(Image *image, s32 startX, s32 startY, s32 width, s32 height)
{
    startX = maximum(0, startX);
    startY = maximum(0, startY);
    s32 endX = minimum(image->width - startX, width);
    s32 endY = minimum(image->height - startY, height);
    u32 *destRow = image->pixels + startY * image->width + startX;

    for (u32 y = startY; y < endY; ++y)
    {
        __m128i *dest = (__m128i *)destRow;
        for (u32 x = startX; x < endX; x += 4)
        {
            if ((x + 3) < endX)
            {
                _mm_storeu_si128(dest++, zero_f32_4x().m);
            }
            else if ((x + 2) < endX)
            {
                u32 *d = (u32 *)dest;
                *d++ = 0;
                *d++ = 0;
                *d++ = 0;
            }
            else if ((x + 1) < endX)
            {
                u32 *d = (u32 *)dest;
                *d++ = 0;
                *d++ = 0;
            }
            else
            {
                u32 *d = (u32 *)dest;
                *d++ = 0;
            }
        }
        destRow += image->width;
    }
}

internal v4_4x
alpha_blend_colours(v4_4x src, v4_4x overlay)
{
    v4_4x result;
    result = overlay + (F32_4x(1.0f) - overlay.a) * src;
    return result;
}

internal v4_4x
unpack_colours(f32_4x colours)
{
    // NOTE(michiel): Expected that colours is filled as u32's.
    f32_4x oneOver255 = F32_4x(1.0f / 255.0f);

    // NOTE(michiel):
    // colours = {
    //     0xB0, 0xG0, 0xR0, 0xA0,
    //     0xB1, 0xG1, 0xR1, 0xA1,
    //     0xB2, 0xG2, 0xR2, 0xA2,
    //     0xB3, 0xG3, 0xR3, 0xA3
    // }
    // a = pshufb(colours, 0xFFFFFF0FFFFFFF0BFFFFFF07FFFFFF03)
    // r = pshufb(colours, 0xFFFFFF0EFFFFFF0AFFFFFF06FFFFFF02)
    // g = pshufb(colours, 0xFFFFFF0DFFFFFF09FFFFFF05FFFFFF01)
    // b = pshufb(colours, 0xFFFFFF0CFFFFFF08FFFFFF04FFFFFF00)

    v4_4x result;
    result.a = f32_4x_from_s32(byte_shuffle(colours, S32_4x(0xFFFFFF03, 0xFFFFFF07, 0xFFFFFF0B, 0xFFFFFF0F)));
    result.r = f32_4x_from_s32(byte_shuffle(colours, S32_4x(0xFFFFFF02, 0xFFFFFF06, 0xFFFFFF0A, 0xFFFFFF0E)));
    result.g = f32_4x_from_s32(byte_shuffle(colours, S32_4x(0xFFFFFF01, 0xFFFFFF05, 0xFFFFFF09, 0xFFFFFF0D)));
    result.b = f32_4x_from_s32(byte_shuffle(colours, S32_4x(0xFFFFFF00, 0xFFFFFF04, 0xFFFFFF08, 0xFFFFFF0C)));
    result = oneOver255 * result;

    return result;
}

internal f32_4x
pack_colours(v4_4x colours)
{
    f32_4x normalizer = F32_4x(255.0f);
    f32_4x result;

    colours = clamp01_4x(colours) * normalizer;
    colours.a = s32_4x_from_f32(colours.a);
    colours.r = s32_4x_from_f32(colours.r);
    colours.g = s32_4x_from_f32(colours.g);
    colours.b = s32_4x_from_f32(colours.b);

    f32_4x alphas = byte_shuffle(colours.a, S32_4x(0x00FFFFFF, 0x04FFFFFF, 0x08FFFFFF, 0x0CFFFFFF));
    f32_4x reds   = byte_shuffle(colours.r, S32_4x(0xFF00FFFF, 0xFF04FFFF, 0xFF08FFFF, 0xFF0CFFFF));
    f32_4x greens = byte_shuffle(colours.g, S32_4x(0xFFFF00FF, 0xFFFF04FF, 0xFFFF08FF, 0xFFFF0CFF));
    f32_4x blues  = byte_shuffle(colours.b, S32_4x(0xFFFFFF00, 0xFFFFFF04, 0xFFFFFF08, 0xFFFFFF0C));

    result = alphas | reds | greens | blues;
    return result;
}

internal f32_4x *
draw_pixels(f32_4x *pixelAt, v4_4x colour)
{
    v4_4x source = unpack_colours(*pixelAt);
    source = alpha_blend_colours(source, colour);
    *pixelAt = pack_colours(source);
    return pixelAt + 1;
}

//
// NOTE(michiel): Lines
//

internal void
draw_line_simd(Image *image, v2 start, v2 end, v4 colourStart = V4(1, 1, 1, 1), v4 colourEnd = V4(1, 1, 1, 1))
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

                u32 *basePixels1 = image->pixels + yPixel1 * image->width + xPixel1;
                u32 *basePixels2 = image->pixels + yPixel2 * image->width + xPixel2;

                v4 source0 = unpack_colour(basePixels1[0]);
                v4 source1 = unpack_colour(basePixels1[image->width]);
                v4 source2 = unpack_colour(basePixels2[0]);
                v4 source3 = unpack_colour(basePixels2[image->width]);

                source0 = source0 * (1.0f - pixel0.a) + pixel0;
                source1 = source1 * (1.0f - pixel1.a) + pixel1;
                source2 = source2 * (1.0f - pixel2.a) + pixel2;
                source3 = source3 * (1.0f - pixel3.a) + pixel3;

                basePixels1[0] = pack_colour(source0);
                basePixels1[image->width] = pack_colour(source1);
                basePixels2[0] = pack_colour(source2);
                basePixels2[image->width] = pack_colour(source3);

                f32_4x interies = F32_4x(intery, intery + 1.0f * gradient,
                                         intery + 2.0f * gradient, intery + 3.0f * gradient);
                f32_4x interyStep = F32_4x(4.0f * gradient);

                f32_4x oneOverCount = F32_4x(1.0f / (f32)(xPixel2 - xPixel1));
                f32_4x xMod = F32_4x(-1.0f, 0.0f, 1.0f, 2.0f);
                f32_4x xPixel1_4x = F32_4x((f32)xPixel1) + xMod;

                for (s32 x = xPixel1 + 1; x < xPixel2; x += 4)
                {
                    v4_4x pixelsA;
                    v4_4x pixelsB;

                    f32_4x t = F32_4x((f32)x) - xPixel1_4x;
                    t = t * oneOverCount;

                    f32_4x fract = fraction(interies);
                    f32_4x oneMinFract = F32_4x(1.0f) - fract;

                    v4_4x lerpColour = lerp(V4_4x(colourStart), t, V4_4x(colourEnd));

                    pixelsA = oneMinFract * lerpColour;
                    pixelsB = fract * lerpColour;

                    u32 offsetA0 = (s32)interies.e[0] * image->width + x + 0;
                    u32 offsetA1 = (s32)interies.e[1] * image->width + x + 1;
                    u32 offsetA2 = (s32)interies.e[2] * image->width + x + 2;
                    u32 offsetA3 = (s32)interies.e[3] * image->width + x + 3;
                    u32 offsetB0 = ((s32)interies.e[0] + 1) * image->width + x + 0;
                    u32 offsetB1 = ((s32)interies.e[1] + 1) * image->width + x + 1;
                    u32 offsetB2 = ((s32)interies.e[2] + 1) * image->width + x + 2;
                    u32 offsetB3 = ((s32)interies.e[3] + 1) * image->width + x + 3;

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

                    v4_4x sourceA = unpack_colours(S32_4x(pixelA0, pixelA1, pixelA2, pixelA3));
                    v4_4x sourceB = unpack_colours(S32_4x(pixelB0, pixelB1, pixelB2, pixelB3));

                    sourceA = alpha_blend_colours(sourceA, pixelsA);
                    sourceB = alpha_blend_colours(sourceB, pixelsB);

                    f32_4x sourcePixA = pack_colours(sourceA);
                    f32_4x sourcePixB = pack_colours(sourceB);

                    image->pixels[offsetA0] = sourcePixA.u[0];
                    image->pixels[offsetB0] = sourcePixB.u[0];

                    if ((x + 1) < xPixel2)
                    {
                        image->pixels[offsetA1] = sourcePixA.u[1];
                        image->pixels[offsetB1] = sourcePixB.u[1];

                        if ((x + 2) < xPixel2)
                        {
                            image->pixels[offsetA2] = sourcePixA.u[2];
                            image->pixels[offsetB2] = sourcePixB.u[2];

                            if ((x + 3) < xPixel2)
                            {
                                image->pixels[offsetA3] = sourcePixA.u[3];
                                image->pixels[offsetB3] = sourcePixB.u[3];
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

                u32 *basePixels1 = image->pixels + yPixel1 * image->width + xPixel1;
                u32 *basePixels2 = image->pixels + yPixel2 * image->width + xPixel2;

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

                for (s32 y = yPixel1 + 1; y < yPixel2; y += 4)
                {
                    v4_4x pixelsA;
                    v4_4x pixelsB;

                    f32_4x t = F32_4x((f32)y) - yPixel1_4x;
                    t = t * oneOverCount;

                    f32_4x fract = fraction(interies);
                    f32_4x oneMinFract = F32_4x(1.0f) - fract;

                    v4_4x lerpColour = lerp(V4_4x(colourStart), t, V4_4x(colourEnd));

                    pixelsA = oneMinFract * lerpColour;
                    pixelsB = fract * lerpColour;

                    u32 offsetA0 = (y + 0) * image->width + (s32)interies.e[0];
                    u32 offsetB0 = (y + 0) * image->width + (s32)interies.e[0] + 1;
                    u32 offsetA1 = (y + 1) * image->width + (s32)interies.e[1];
                    u32 offsetB1 = (y + 1) * image->width + (s32)interies.e[1] + 1;
                    u32 offsetA2 = (y + 2) * image->width + (s32)interies.e[2];
                    u32 offsetB2 = (y + 2) * image->width + (s32)interies.e[2] + 1;
                    u32 offsetA3 = (y + 3) * image->width + (s32)interies.e[3];
                    u32 offsetB3 = (y + 3) * image->width + (s32)interies.e[3] + 1;

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

                    v4_4x sourceA = unpack_colours(S32_4x(pixelA0, pixelA1, pixelA2, pixelA3));
                    v4_4x sourceB = unpack_colours(S32_4x(pixelB0, pixelB1, pixelB2, pixelB3));

                    sourceA = alpha_blend_colours(sourceA, pixelsA);
                    sourceB = alpha_blend_colours(sourceB, pixelsB);

                    f32_4x sourcePixA = pack_colours(sourceA);
                    f32_4x sourcePixB = pack_colours(sourceB);

                    image->pixels[offsetA0] = sourcePixA.u[0];
                    image->pixels[offsetB0] = sourcePixB.u[0];

                    if ((y + 1) < yPixel2)
                    {
                        image->pixels[offsetA1] = sourcePixA.u[1];
                        image->pixels[offsetB1] = sourcePixB.u[1];

                        if ((y + 2) < yPixel2)
                        {
                            image->pixels[offsetA2] = sourcePixA.u[2];
                            image->pixels[offsetB2] = sourcePixB.u[2];

                            if ((y + 3) < yPixel2)
                            {
                                image->pixels[offsetA3] = sourcePixA.u[3];
                                image->pixels[offsetB3] = sourcePixB.u[3];
                            }
                        }
                    }
                    interies = interies + interyStep;
                }
            }
        }
    }
}

internal void
draw_lines_simd(Image *image, u32 pointCount, v2 *points, v4 *colours, v2 offset, v2 scale = V2(1, 1))
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
        draw_line_simd(image, prevP, P, colours[pointIdx - 1], colours[pointIdx]);
        prevP = P;
    }
}

internal void
draw_lines_simd(Image *image, u32 pointCount, ColouredPoint *points, v2 offset, v2 scale = V2(1, 1))
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
        draw_line_simd(image, prevP.point, P.point, prevP.colour, P.colour);
        prevP = P;
    }
}

internal void
fill_rect_simd(Image *image, f32 xStart, f32 yStart, f32 width, f32 height, v4 colour)
{
    if (xStart < 0.0f)
    {
        f32 diff = -xStart;
        f32 newWidth = width - diff;
        if (newWidth < 0.0f)
        {
            newWidth = 0.0f;
        }
        width = newWidth;
        xStart = 0.0f;
    }
    else if ((xStart + width) > (f32)image->width)
    {
        f32 diff = (xStart + width) - (f32)image->width;
        f32 newWidth = width - diff;
        if (newWidth < 0.0f)
        {
            newWidth = 0.0f;
        }
        width = newWidth;
    }

    if (yStart < 0.0f)
    {
        f32 diff = -yStart;
        f32 newHeight = height - diff;
        if (newHeight < 0.0f)
        {
            newHeight = 0.0f;
        }
        height = newHeight;
        yStart = 0.0f;
    }
    else if ((yStart + height) > (f32)image->height)
    {
        f32 diff = (yStart + height) - (f32)image->height;
        f32 newHeight = height - diff;
        if (newHeight < 0.0f)
        {
            newHeight = 0.0f;
        }
        height = newHeight;
    }

    u32 xMax = u32_from_f32_round(width);
    u32 xMax4 = xMax >> 2;
    u32 xMaxRem = xMax & 0x3;

    colour.rgb *= colour.a;
    v4_4x colours = V4_4x(colour);

    u32 *pixelAt = image->pixels + u32_from_f32_floor(yStart * image->width) + u32_from_f32_floor(xStart);
    for (u32 y = 0; y < u32_from_f32_round(height); ++y)
    {
        f32_4x *pixelRow = (f32_4x *)pixelAt;
        for (u32 x = 0; x < xMax4; ++x)
        {
            pixelRow = draw_pixels(pixelRow, colours);
        }
        u32 *pixelRem = (u32 *)pixelRow;
        for (u32 x = 0; x < xMaxRem; ++x)
        {
            pixelRem = draw_pixel(pixelRem, colour);
        }
        pixelAt += image->width;
    }
}

internal void
fill_rect_simd(Image *image, v2 topLeft, v2 dim, v4 colour)
{
    fill_rect_simd(image, topLeft.x, topLeft.y, dim.x, dim.y, colour);
}
