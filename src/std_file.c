internal READ_ENTIRE_FILE(read_entire_file)
{
    Buffer result = {0};

    FILE *input = fopen(to_cstring(filename), "rb");
    if (input)
    {
        fseek(input, 0, SEEK_END);
        result.size = ftell(input);
        fseek(input, 0, SEEK_SET);

        result.data = (u8 *)allocate_size(result.size, 0);
        i_expect(result.data);
        // TODO(michiel): Checking read size
        fread(result.data, 1, result.size, input);
        fclose(input);
    }

    return result;
}

internal WRITE_ENTIRE_FILE(write_entire_file)
{
    b32 result = false;
    FILE *output = fopen(to_cstring(filename), "wb");
    if (output)
    {
        // TODO(michiel): Checking write size
        fwrite(buffer.data, 1, buffer.size, output);
        result = true;
        fclose(output);
    }
    return result;
}

internal OPEN_FILE(open_file)
{
    ApiFile result = {0};

    char *openMode = "rb";
    if (flags & FileOpen_Write)
    {
        if (flags & FileOpen_Read)
        {
            i_expect(!"R/W Not yet supported!");
        }
        else
        {
            openMode = "wb";
        }
    }

    FILE *f = fopen(to_cstring(filename), openMode);
    result.platform = f;

    return result;
}

internal GET_FILE_SIZE(get_file_size)
{
    umm size = 0;
    FILE *stdFile = (FILE *)apiFile->platform;
    if (stdFile) {
        umm oldPos = ftell(stdFile);
        fseek(stdFile, 0, SEEK_END);
        size = ftell(stdFile);
        fseek(stdFile, oldPos, SEEK_SET);
    }
    return size;
}

internal READ_FROM_FILE(read_from_file)
{
    umm result = 0;
    FILE *inFile = (FILE *)apiFile->platform;
    result = fread(buffer, 1, size, inFile);
    return result;
}

internal READ_FROM_FILE_OFFSET(read_from_file_offset)
{
    umm result = 0;
    FILE *inFile = (FILE *)apiFile->platform;
    fseek(inFile, offset, SEEK_SET);
    result = fread(buffer, 1, size, inFile);
    return result;
}

internal WRITE_TO_FILE(write_to_file)
{
    FILE *outFile = (FILE *)apiFile->platform;
    fwrite(data, 1, size, outFile);
}

internal WRITE_VFMT_TO_FILE(write_vfmt_to_file)
{
    FILE *outFile = (FILE *)apiFile->platform;
    vfprintf(outFile, fmt, args);
}

internal WRITE_FMT_TO_FILE(write_fmt_to_file)
{
    va_list args;
    va_start(args, fmt);
    write_vfmt_to_file(apiFile, fmt, args);
    va_end(args);
}

internal CLOSE_FILE(close_file)
{
    FILE *f = (FILE *)apiFile->platform;
    if (f)
    {
        fclose(f);
    }
    apiFile->platform = 0;
}
