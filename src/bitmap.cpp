#pragma pack(push, 1)
struct BitmapHeader
{
    u16 fileType;
    u32 fileSize;
    u16 reserved1;
    u16 reserved2;
    u32 bitmapOffset;
    u32 size;
    s32 width;
    s32 height;
    u16 planes;
    u16 bitsPerPixel;
    u32 compression;
    u32 sizeOfBitmap;
    s32 horzResolution;
    s32 vertResolution;
    u32 coloursUsed;
    u32 coloursImportant;
    
    u32 redMask;
    u32 greenMask;
    u32 blueMask;
};
#pragma pack(pop)

struct BitScanResult
{
    b32 found;
    u32 index;
};

inline BitScanResult
find_least_significant_set_bit(u32 value)
{
    BitScanResult result = {};
    
    for (s32 test = 0; test < 32; ++test)
    {
        if (value & (1 << test))
        {
            result.index = test;
            result.found = true;
            break;
        }
    }

    return result;
}

internal u32
get_total_pixel_size(Image *image)
{
    u32 result = image->width * image->height * sizeof(u32);
    return result;
}

internal u32 *
get_pixel_pointer(Image *image, u32 x, u32 y)
{
    u32 *result = image->pixels + y*image->width + x;
    return result;
}

internal Image
load_bitmap(API *api, char *filename)
{
    Image result = {};
    
     ApiFile readResult = api->file.read_entire_file(filename);
    
    if (readResult.content.size != 0)
    {
        BitmapHeader *header = (BitmapHeader *)readResult.content.data;
        u32 *pixels = (u32 *)(readResult.content.data + header->bitmapOffset);
        result.pixels = pixels;
        result.width = header->width;
        result.height = header->height;
        
        i_expect(result.height >= 0);
        i_expect(header->compression == 3);
        
        // NOTE(casey): If you are using this generically for some reason,
        // please remember that BMP files CAN GO IN EITHER DIRECTION and
        // the height will be negative for top-down.
        // (Also, there can be compression, etc., etc... DON'T think this
        // is complete BMP loading code because it isn't!!)
        
        // NOTE(casey): Byte order in memory is determined by the Header itself,
        // so we have to read out the masks and convert the pixels ourselves.
        u32 redMask = header->redMask;
        u32 greenMask = header->greenMask;
        u32 blueMask = header->blueMask;
        u32 alphaMask = ~(redMask | greenMask | blueMask);        
        
        BitScanResult redScan = find_least_significant_set_bit(redMask);
        BitScanResult greenScan = find_least_significant_set_bit(greenMask);
        BitScanResult blueScan = find_least_significant_set_bit(blueMask);
        BitScanResult alphaScan = find_least_significant_set_bit(alphaMask);
        
        i_expect(redScan.found);
        i_expect(greenScan.found);
        i_expect(blueScan.found);
        i_expect(alphaScan.found);
        
        s32 redShiftDown = (s32)redScan.index;
        s32 greenShiftDown = (s32)greenScan.index;
        s32 blueShiftDown = (s32)blueScan.index;
        s32 alphaShiftDown = (s32)alphaScan.index;
        
        u32 *dest = pixels;
        for(s32 y = 0; y < header->height; ++y)
        {
            for(s32 x = 0; x < header->width; ++x)
            {
                u32 c = *dest;
                
                *dest++ = ((((c & alphaMask) >> alphaShiftDown) << 24) |
                           (((c & blueMask) >> blueShiftDown)   << 16) |
                           (((c & greenMask) >> greenShiftDown) <<  8) |
                           (((c & redMask) >> redShiftDown)     <<  0));
            }
        }
    }
    
    return result;
}

internal void
write_bitmap(API *api, Image *image, char *outputFilename)
{
    u32 outputPixelSize = get_total_pixel_size(image);
    
    BitmapHeader header = {};
    header.fileType = 0x4D42;
    header.fileSize = sizeof(header) + outputPixelSize;
    header.bitmapOffset = sizeof(header);
    header.size = sizeof(header) - 14;
    header.width = image->width;
    header.height = image->height;
    header.planes = 1;
    header.bitsPerPixel = 32;
    header.compression = 0;
    header.sizeOfBitmap = outputPixelSize;
    header.horzResolution = 0;
    header.vertResolution = 0;
    header.coloursUsed = 0;
    header.coloursImportant = 0;
    
    ApiFile file = api->file.open_file(outputFilename, FileOpen_Write);
    if (file.handle)
    {
    api->file.write_to_file(file, sizeof(header), &header);
        api->file.write_to_file(file, outputPixelSize, image->pixels);
        api->file.close_file(&file);
    }
    }
