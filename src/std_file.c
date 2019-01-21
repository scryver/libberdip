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
        fread(result.content.data, result.content.size, 1, input);
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
        fwrite(data, size, 1, output);
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

internal READ_FROM_FILE(read_from_file)
{
    FILE *inFile = (FILE *)file.handle;
      fread(buffer, size, 1, inFile);
}

internal WRITE_TO_FILE(write_to_file)
{
    FILE *outFile = (FILE *)file.handle;
    fwrite(data, size, 1, outFile);
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
