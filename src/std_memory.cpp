//
// NOTE(michiel): Standard library memory
//

internal void
std_deallocate_memory_block(StdMemoryBlock *block)
{
    begin_ticket_mutex(&gStdMemory.mutex);
    block->next->prev = block->prev;
    block->prev->next = block->next;
    end_ticket_mutex(&gStdMemory.mutex);

    free(block);
}

internal PLATFORM_ALLOCATE_MEMORY(std_allocate_memory)
{
    umm totalSize = size + sizeof(StdMemoryBlock);
    umm baseOffset = sizeof(StdMemoryBlock);

    StdMemoryBlock *block = 0;
    if (flags & Memory_NoClear)
    {
        block = (StdMemoryBlock *)malloc(totalSize);
    }
    else
    {
        block = (StdMemoryBlock *)calloc(totalSize, 1);
    }
    // TODO(michiel): Errors
    i_expect(block);

    block->block.base = (u8 *)block + baseOffset;
    block->block.used = 0;
    block->block.prev = 0;

    StdMemoryBlock *sentinel = &gStdMemory.sentinel;
    block->next = sentinel;
    block->block.size = size;

    begin_ticket_mutex(&gStdMemory.mutex);
    block->prev = sentinel;
    block->prev->next = block;
    block->next->prev = block;
    end_ticket_mutex(&gStdMemory.mutex);

    PlatformMemoryBlock *result = &block->block;
    return result;
}

internal PLATFORM_REALLOCATE_MEMORY(std_reallocate_memory)
{
    PlatformMemoryBlock *result = 0;
    if (block)
    {
        StdMemoryBlock *stdHandle = (StdMemoryBlock *)block;
        umm oldSize = block->size + sizeof(StdMemoryBlock);
        umm totalSize = newSize + sizeof(StdMemoryBlock);
        void *newMem = realloc(stdHandle, totalSize);
        i_expect(newMem);

        StdMemoryBlock *newHandle = (StdMemoryBlock *)newMem;
        i_expect((newHandle->block.size + sizeof(StdMemoryBlock)) == oldSize);
        newHandle->block.size = newSize;
        result = &newHandle->block;
    }
    else
    {
        result = std_allocate_memory(newSize, flags);
    }
    return result;
}

internal PLATFORM_DEALLOCATE_MEMORY(std_deallocate_memory)
{
    if (block)
    {
        StdMemoryBlock *stdHandle = (StdMemoryBlock *)block;
        std_deallocate_memory_block(stdHandle);
    }
    return 0;
}

internal PLATFORM_DEALLOCATE_ALL_MEMORY(std_deallocate_all_memory)
{
    StdMemoryBlock *sentinel = &gStdMemory.sentinel;
    while (sentinel->prev != sentinel)
    {
        // NOTE(michiel): Walk backwards
        std_deallocate_memory_block(sentinel->prev);
    }
}

internal PLATFORM_EXECUTABLE_MEMORY(std_executable_memory)
{
    b32 result = false;
    return result;
}

internal INIT_MEMORY_API(std_memory_api)
{
    gStdMemory.sentinel.next = gStdMemory.sentinel.prev = &gStdMemory.sentinel;
    memoryApi->allocate_memory   = std_allocate_memory;
    memoryApi->reallocate_memory = std_reallocate_memory;
    memoryApi->deallocate_memory = std_deallocate_memory;
    memoryApi->deallocate_all    = std_deallocate_all_memory;
    memoryApi->executable_memory = std_executable_memory;
}

//
// NOTE(michiel): Std allocator without platform support, no alignment support
//

internal ALLOCATE_MEMORY_SIZE(std_allocate_size)
{
    if (!(flags & Memory_AlignMask))
    {
        flags |= MEMORY_DEFAULT_ALIGN;
    }

    u32 alignment = (flags & Memory_AlignMask);
    i_expect(alignment <= 16);
    i_expect(is_pow2(alignment));

    void *result = 0;
    if (flags & Memory_NoClear)
    {
        result = malloc(size);
    }
    else
    {
        umm divSize = size / alignment;
        if (divSize * alignment < size)
        {
            ++divSize;
        }
        // NOTE(michiel): Don't know if it is better this way, but a _good_ implementation should align the memory
        // to the second argument.
        result = calloc(divSize, alignment);
    }
    i_expect(result);
    i_expect(((umm)result & (alignment - 1)) == 0);

    return result;
}

internal ALLOCATE_MEMORY_COPY(std_allocate_copy)
{
    flags |= Memory_NoClear;
    void *result = std_allocate_size(0, size, flags);
    copy(size, source, result);
    return result;
}

internal ALLOCATE_MEMORY_STRINGZ(std_allocate_stringz)
{
    String result;
    result.size = 0;
    result.data = (u8 *)std_allocate_size(0, source.size + 1, align_memory_alloc(1, false));
    if (result.data)
    {
        result.size = source.size;
        copy(source.size, source.data, result.data);
        result.data[result.size] = 0;
    }
    return result;
}

internal REALLOCATE_MEMORY_SIZE(std_reallocate_size)
{
    void *result = realloc(memory, size);
    i_expect(result);
    return result;
}

internal DEALLOCATE_MEMORY(std_deallocate)
{
    if (memory)
    {
        free(memory);
    }
    return 0;
}

internal INIT_ALLOCATOR(initialize_std_allocator)
{
    dest->allocator = 0;
    dest->bootstrap_alloc = 0;
    dest->allocate_size = std_allocate_size;
    dest->allocate_copy = std_allocate_copy;
    dest->allocate_stringz = std_allocate_stringz;
    dest->reallocate_size = std_reallocate_size;
    dest->deallocate = std_deallocate;
    dest->deallocate_all = 0;
}
