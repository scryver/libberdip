global const char gBase64Map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

global const u32 gDecode64Map[256] = {
    ['A'] = 0, ['B'] = 1, ['C'] = 2, ['D'] = 3, ['E'] = 4, ['F'] = 5, ['G'] = 6,
    ['H'] = 7, ['I'] = 8, ['J'] = 9, ['K'] = 10, ['L'] = 11, ['M'] = 12,
    ['N'] = 13, ['O'] = 14, ['P'] = 15, ['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19,
    ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23, ['Y'] = 24, ['Z'] = 25,
    ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31, ['g'] = 32,
    ['h'] = 33, ['i'] = 34, ['j'] = 35, ['k'] = 36, ['l'] = 37, ['m'] = 38,
    ['n'] = 39, ['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43, ['s'] = 44, ['t'] = 45,
    ['u'] = 46, ['v'] = 47, ['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51,
    ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55, ['4'] = 56,
    ['5'] = 57, ['6'] = 58, ['7'] = 59, ['8'] = 60, ['9'] = 61,
    ['+'] = 62, ['/'] = 63,
};

internal b32
base64_encode(umm sourceLength, u8 *source, umm destLength, u8 *dest)
{
    umm outputLength = 4 * ((sourceLength + 2) / 3);
    b32 error = false;
    if (source && dest)
    {
        for (u32 srcIndex = 0, destIndex = 0; srcIndex < sourceLength; )
        {
            u32 octetA = srcIndex < sourceLength ? source[srcIndex++] : 0;
            u32 octetB = srcIndex < sourceLength ? source[srcIndex++] : 0;
            u32 octetC = srcIndex < sourceLength ? source[srcIndex++] : 0;
            
            u32 triplet = (octetA << 16) | (octetB << 8) | octetC;
            
            i_expect(destIndex < destLength);
            dest[destIndex++] = gBase64Map[(triplet >> (3 * 6)) & 0x3F];
            i_expect(destIndex < destLength);
            dest[destIndex++] = gBase64Map[(triplet >> (2 * 6)) & 0x3F];
            i_expect(destIndex < destLength);
            dest[destIndex++] = gBase64Map[(triplet >> (1 * 6)) & 0x3F];
            i_expect(destIndex < destLength);
            dest[destIndex++] = gBase64Map[(triplet >> (0 * 6)) & 0x3F];
        }
        
        u32 extraEquals = 3 - (sourceLength % 3);
        if (extraEquals == 3)
        {
            extraEquals = 0;
        }
        
        for (u32 destIndex = 0; destIndex < extraEquals; ++destIndex)
        {
            dest[outputLength - destIndex - 1] = '=';
        }
        i_expect(outputLength < destLength);
        dest[outputLength] = 0;
    }
    else
    {
        error = true;
    }
    return error;
}

internal b32
base64_decode(umm sourceLength, u8 *source, umm destLength, u8 *dest)
{
    b32 error = false;
    
    if (source && dest)
    {
        i_expect((sourceLength & ~0x3) == sourceLength);
        
        u32 dstIndex = 0;
        for (u32 srcIndex = 0; srcIndex < sourceLength; srcIndex += 4)
        {
            u32 index0 = gDecode64Map[(u32)source[srcIndex + 0]];
            u32 index1 = gDecode64Map[(u32)source[srcIndex + 1]];
            u32 index2 = gDecode64Map[(u32)source[srcIndex + 2]];
            u32 index3 = gDecode64Map[(u32)source[srcIndex + 3]];
            
            u32 triplet = ((index3 << (0 * 6)) |
                           (index2 << (1 * 6)) |
                           (index1 << (2 * 6)) |
                           (index0 << (3 * 6)));
            u32 octetA = (triplet >> 16) & 0xFF;
            u32 octetB = (triplet >>  8) & 0xFF;
            u32 octetC = (triplet >>  0) & 0xFF;
            
            i_expect((dstIndex + 2) < destLength);
            dest[dstIndex++] = octetA;
            dest[dstIndex++] = octetB;
            dest[dstIndex++] = octetC;
        }
        i_expect(dstIndex < destLength);
        dest[dstIndex] = 0;
    }
    else
    {
        error = true;
    }
    
    return error;
}
