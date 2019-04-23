internal READ_ENTIRE_FILE(read_entire_file)
{
    ApiFile result = {0};
    
    FILE *input = fopen(filename, "rb");
    if (input)
    {
        fseek(input, 0, SEEK_END);
        result.content.size = ftell(input);
        fseek(input, 0, SEEK_SET);
        
        result.content.data = (u8 *)allocate_size(result.content.size, 0);
        i_expect(result.content.data);
        // TODO(michiel): Checking read size
        fread(result.content.data, 1, result.content.size, input);
        fclose(input);
    }
    
    return result;
}

internal WRITE_ENTIRE_FILE(write_entire_file)
{
    FILE *output = fopen(filename, "wb");
    if (output)
    {
        // TODO(michiel): Checking write size
        fwrite(data, 1, size, output);
        fclose(output);
    }
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
    
    FILE *f = fopen(filename, openMode);
    result.handle = (u64)f;
    
    return result;
}

internal GET_FILE_SIZE(get_file_size)
{
    umm size = 0;
    FILE *stdFile = (FILE *)file.handle;
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
    FILE *inFile = (FILE *)file.handle;
    result = fread(buffer, 1, size, inFile);
    return result;
}

internal WRITE_TO_FILE(write_to_file)
{
    FILE *outFile = (FILE *)file.handle;
    fwrite(data, 1, size, outFile);
}

internal WRITE_VFMT_TO_FILE(write_vfmt_to_file)
{
    FILE *outFile = (FILE *)file.handle;
    vfprintf(outFile, fmt, args);
}

internal WRITE_FMT_TO_FILE(write_fmt_to_file)
{
    va_list args;
    va_start(args, fmt);
    write_vfmt_to_file(file, fmt, args);
    va_end(args);
}

internal CLOSE_FILE(close_file)
{
    FILE *f = (FILE *)file->handle;
    if (f)
    {
        fclose(f);
    }
    if (file->content.size && file->content.data)
    {
        deallocate(file->content.data);
    }
    file->handle = 0;
    file->content.size = 0;
    file->content.data = 0;
}
