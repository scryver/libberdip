internal CircularBuffer *
alloc_circular_buffer(MemoryAllocator *allocator, u32 size)
{
    i_expect(size >= 8);
    i_expect(is_pow2(size));

    umm totalSize = offset_of(CircularBuffer, buffer) + size;
    CircularBuffer *result = (CircularBuffer *)allocate_size(allocator, totalSize, default_memory_alloc());
    result->mask = size - 1;
    return result;
}

// NOTE(michiel): returns number of bytes written
internal u32
circular_add(CircularBuffer *buffer, u32 sourceCount, u8 *source)
{
    u32 result = 0;
    u32 available = get_available_size(buffer);
    if (available)
    {
        u32 dataCount = sourceCount > available ? available : sourceCount;

        u32 writeIndex = buffer->writeIndex;
        u32 endIndex   = (buffer->writeIndex + dataCount) & buffer->mask;
        if (endIndex > writeIndex)
        {
            i_expect(endIndex - writeIndex == dataCount);
            copy(dataCount, source, buffer->buffer + writeIndex);
        }
        else
        {
            u32 tillEnd = buffer->mask + 1 - writeIndex;
            i_expect(tillEnd + endIndex == dataCount);
            copy(tillEnd, source, buffer->buffer + writeIndex);
            copy(endIndex, source + tillEnd, buffer->buffer);
        }
        buffer->writeIndex = endIndex;
        result = dataCount;
    }
    return result;
}

// NOTE(michiel): returns number of bytes read
internal u32
circular_get(CircularBuffer *buffer, u32 maxDataCount, u8 *dest)
{
    u32 result = 0;
    u32 currentSize = get_size(buffer);
    if (currentSize)
    {
        u32 dataCount = maxDataCount > currentSize ? currentSize : maxDataCount;

        u32 readIndex = buffer->readIndex;
        u32 endIndex  = (buffer->readIndex + dataCount) & buffer->mask;
        if (endIndex > readIndex)
        {
            i_expect(endIndex - readIndex == dataCount);
            copy(dataCount, buffer->buffer + readIndex, dest);
        }
        else
        {
            u32 tillEnd = buffer->mask + 1 - readIndex;
            i_expect(tillEnd + endIndex == dataCount);
            copy(tillEnd, buffer->buffer + readIndex, dest);
            copy(endIndex, buffer->buffer, dest + tillEnd);
        }
        buffer->readIndex = endIndex;
        result = dataCount;
    }
    return result;
}

internal void
test_circular_buffer(MemoryAllocator *allocator)
{
    CircularBuffer *testBuf = alloc_circular_buffer(allocator, 128);

    u8 filler[128];
    copy_single(128, 0x55, filler);

    u8 output[128];
    u32 gotten = circular_get(testBuf, array_count(output), output);
    i_expect(gotten == 0);
    i_expect(get_size(testBuf) == 0);
    i_expect(get_available_size(testBuf) == testBuf->mask);

    u32 putIn = circular_add(testBuf, array_count(filler), filler);
    i_expect(putIn == testBuf->mask);
    i_expect(get_size(testBuf) == testBuf->mask);
    i_expect(get_available_size(testBuf) == 0);

    gotten = circular_get(testBuf, array_count(output), output);
    i_expect(gotten == testBuf->mask);
    i_expect(get_size(testBuf) == 0);
    i_expect(get_available_size(testBuf) == testBuf->mask);

    putIn = circular_add(testBuf, array_count(filler), filler);
    i_expect(putIn == testBuf->mask);
    i_expect(get_size(testBuf) == testBuf->mask);
    i_expect(get_available_size(testBuf) == 0);

    gotten = circular_get(testBuf, array_count(output), output);
    i_expect(gotten == testBuf->mask);
    i_expect(get_size(testBuf) == 0);
    i_expect(get_available_size(testBuf) == testBuf->mask);

    putIn = circular_add(testBuf, array_count(filler), filler);
    i_expect(putIn == testBuf->mask);
    i_expect(get_size(testBuf) == testBuf->mask);
    i_expect(get_available_size(testBuf) == 0);

    gotten = circular_get(testBuf, array_count(output), output);
    i_expect(gotten == testBuf->mask);
    i_expect(get_size(testBuf) == 0);
    i_expect(get_available_size(testBuf) == testBuf->mask);

    i_expect(testBuf->readIndex == testBuf->writeIndex);
    i_expect(testBuf->readIndex == testBuf->mask + 1 - 3); // NOTE(michiel): -3 for the 3 fill/clears

    u8 testArray[39];
    u8 outArray[39];
    for (u32 testIdx = 0; testIdx < 120; ++testIdx)
    {
        for (u32 x = 0; x < array_count(testArray); ++x) {
            testArray[x] = x + 3 * x - 5;
        }

        u32 a = circular_add(testBuf, 13, testArray);
        u32 b = circular_add(testBuf, 13, testArray + 13);
        u32 c = circular_add(testBuf, 13, testArray + 26);
        fprintf(stdout, "Added %u %u %u\n", a, b, c);

        if (get_size(testBuf) > 39)
        {
            u32 gotten = circular_get(testBuf, array_count(outArray), outArray);
            i_expect(gotten <= array_count(outArray));
            for (u32 index = 0; index < gotten; ++index)
            {
                s32 diff = testArray[index % 39] - outArray[index];
                if (diff) {
                    fprintf(stdout, "%u vs %u\n", testArray[index % 39], outArray[index]);
                }
            }
        }
    }
}
