
//
// NOTE(michiel): Linux memory
//

internal void
linux_deallocate_memory_block(LinuxMemoryBlock *block)
{
    umm size = block->block.size;
    umm totalSize = size + sizeof(LinuxMemoryBlock);

    begin_ticket_mutex(&gLinuxMemory.mutex);
    block->next->prev = block->prev;
    block->prev->next = block->next;
    end_ticket_mutex(&gLinuxMemory.mutex);

    munmap(block, totalSize);
}

internal PLATFORM_ALLOCATE_MEMORY(linux_allocate_memory)
{
    // TODO(michiel): Use flags maybe? Alignment is always page aligned, and memory is always cleared to zero...
    //s64 pageSize = sysconf(_SC_PAGESIZE);
    umm totalSize = size + sizeof(LinuxMemoryBlock);
    umm baseOffset = sizeof(LinuxMemoryBlock);

    LinuxMemoryBlock *block = (LinuxMemoryBlock *)mmap(0, totalSize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    // TODO(michiel): Errors
    i_expect(block && (block != MAP_FAILED));

    block->block.base = (u8 *)block + baseOffset;
    i_expect(block->block.used == 0);
    i_expect(block->block.prev == 0);

    LinuxMemoryBlock *sentinel = &gLinuxMemory.sentinel;
    block->next = sentinel;
    block->block.size = size;

    begin_ticket_mutex(&gLinuxMemory.mutex);
    block->prev = sentinel;
    block->prev->next = block;
    block->next->prev = block;
    end_ticket_mutex(&gLinuxMemory.mutex);

    PlatformMemoryBlock *result = &block->block;
    return result;
}

internal PLATFORM_REALLOCATE_MEMORY(linux_reallocate_memory)
{
    PlatformMemoryBlock *result = 0;
    if (block)
    {
        LinuxMemoryBlock *linuxHandle = (LinuxMemoryBlock *)block;
        umm oldSize = block->size + sizeof(LinuxMemoryBlock);
        umm totalSize = newSize + sizeof(LinuxMemoryBlock);
        // TODO(michiel): Make a stable pointer version?
        void *newMem = mremap(linuxHandle, linuxHandle->block.size + sizeof(LinuxMemoryBlock), totalSize, MREMAP_MAYMOVE);
        i_expect(newMem && (newMem != MAP_FAILED));

        LinuxMemoryBlock *newHandle = (LinuxMemoryBlock *)newMem;
        i_expect((newHandle->block.size + sizeof(LinuxMemoryBlock)) == oldSize);
        newHandle->block.size = newSize;

        result = &newHandle->block;
    }
    else
    {
        result = linux_allocate_memory(newSize, flags);
    }
    return result;
}

internal PLATFORM_DEALLOCATE_MEMORY(linux_deallocate_memory)
{
    if (block)
    {
        LinuxMemoryBlock *linuxHandle = (LinuxMemoryBlock *)block;
        linux_deallocate_memory_block(linuxHandle);
    }
    return 0;
}

internal PLATFORM_DEALLOCATE_ALL_MEMORY(linux_deallocate_all_memory)
{
    LinuxMemoryBlock *sentinel = &gLinuxMemory.sentinel;
    while (sentinel->prev != sentinel)
    {
        // NOTE(michiel): Walk backwards
        linux_deallocate_memory_block(sentinel->prev);
    }
}

internal PLATFORM_EXECUTABLE_MEMORY(linux_executable_memory)
{
    b32 result = false;

    if (block)
    {
        s64 pageSize = sysconf(_SC_PAGESIZE);
        umm pageOffset = offset ? offset + sizeof(LinuxMemoryBlock) : 0;
        pageOffset = align_up(pageOffset, pageSize);
        LinuxMemoryBlock *linuxHandle = (LinuxMemoryBlock *)block;
        s32 protResult = mprotect((u8 *)linuxHandle + pageOffset, linuxHandle->block.size + sizeof(LinuxMemoryBlock) - pageOffset, PROT_READ|PROT_EXEC);
        result = protResult == 0;
    }

    return result;
}

internal INIT_MEMORY_API(linux_memory_api)
{
    gLinuxMemory.sentinel.next = gLinuxMemory.sentinel.prev = &gLinuxMemory.sentinel;
    memoryApi->allocate_memory   = linux_allocate_memory;
    memoryApi->reallocate_memory = linux_reallocate_memory;
    memoryApi->deallocate_memory = linux_deallocate_memory;
    memoryApi->deallocate_all    = linux_deallocate_all_memory;
    memoryApi->executable_memory = linux_executable_memory;
}
