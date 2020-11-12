//
// NOTE(michiel): Platform builtin allocator (use for easy platform allocations, used with big data sizes)
//

// TODO(michiel): Change all pointer use of gMemoryApi to direct usage

#ifndef MEMORY_PLATFORM_REAL_SIZE
#error MEMORY_PLATFORM_REAL_SIZE not defined, make sure to include a platform allocator
#endif

internal ALLOCATE_MEMORY_SIZE(platform_allocate_size)
{
    if (!(flags & Memory_AlignMask))
    {
        flags |= MEMORY_DEFAULT_ALIGN;
    }

    u32 alignment = (flags & Memory_AlignMask);
    i_expect(is_pow2(alignment));

    void *result = 0;

    PlatformMemoryBlock *block = gMemoryApi->allocate_memory(size, flags);
    i_expect(block);
    i_expect(((umm)block->base - (umm)block) == MEMORY_PLATFORM_REAL_SIZE);
    result = (void *)block->base;

    i_expect(((umm)result & (alignment - 1)) == 0);

    return result;
}

internal ALLOCATE_MEMORY_COPY(platform_allocate_copy)
{
    flags |= Memory_NoClear;
    void *result = platform_allocate_size(allocator, size, flags);
    copy(size, source, result);
    return result;
}

internal ALLOCATE_MEMORY_STRINGZ(platform_allocate_stringz)
{
    String result;
    result.size = 0;
    result.data = (u8 *)platform_allocate_size(allocator, source.size + 1, align_memory_alloc(1, false));
    if (result.data)
    {
        result.size = source.size;
        copy(source.size, source.data, result.data);
        result.data[result.size] = 0;
    }
    return result;
}

internal REALLOCATE_MEMORY_SIZE(platform_reallocate_size)
{
    PlatformMemoryBlock *oldBlock = 0;
    if (memory)
    {
        oldBlock = (PlatformMemoryBlock *)((u8 *)memory - MEMORY_PLATFORM_REAL_SIZE);
    }
    PlatformMemoryBlock *newBlock = gMemoryApi->reallocate_memory(oldBlock, size, flags);
    i_expect(newBlock);
    void *result = (void *)newBlock->base;
    return result;
}

internal DEALLOCATE_MEMORY(platform_deallocate)
{
    PlatformMemoryBlock *oldBlock = 0;
    if (memory)
    {
        oldBlock = (PlatformMemoryBlock *)((u8 *)memory - MEMORY_PLATFORM_REAL_SIZE);
        oldBlock = gMemoryApi->deallocate_memory(oldBlock);
        memory = oldBlock ? oldBlock->base : 0;
    }
    return memory;
}

internal DEALLOCATE_ALL(platform_deallocate_all)
{
    gMemoryApi->deallocate_all();
}

internal INIT_ALLOCATOR(initialize_platform_allocator)
{
    dest->allocator = allocator;
    dest->bootstrap_alloc = 0;
    dest->allocate_size = platform_allocate_size;
    dest->allocate_copy = platform_allocate_copy;
    dest->allocate_stringz = platform_allocate_stringz;
    dest->reallocate_size = platform_reallocate_size;
    dest->deallocate = platform_deallocate;
    dest->deallocate_all = platform_deallocate_all;
}

//
// NOTE(michiel): Arenas
//

internal umm
get_alignment_offset(MemoryArena *arena, umm alignment)
{
    i_expect(is_pow2(alignment));
    i_expect(arena->currentBlock);
    umm result = 0;

    umm resultPtr = (umm)arena->currentBlock->base + arena->currentBlock->used;
    umm alignmentMask = alignment - 1;
    if (resultPtr & alignmentMask)
    {
        result = alignment - (result & alignmentMask);
    }

    return result;
}

internal umm
get_aligned_size_for(MemoryArena *arena, umm size, u32 flags)
{
    umm alignmentOffset = get_alignment_offset(arena, flags & Memory_AlignMask);
    return size + alignmentOffset;
}

internal ALLOCATE_MEMORY_SIZE(arena_allocate_size)
{
    if (!(flags & Memory_AlignMask))
    {
        flags |= MEMORY_DEFAULT_ALIGN;
    }

    u32 alignment = (flags & Memory_AlignMask);
    i_expect(alignment <= 128);
    i_expect(is_pow2(alignment));

    MemoryArena *arena = (MemoryArena *)allocator;
    void *result = 0;

    umm totalSize = 0;
    if (arena->currentBlock)
    {
        totalSize = get_aligned_size_for(arena, size, flags);
    }

    if (!arena->currentBlock ||
        ((arena->currentBlock->used + totalSize) > arena->currentBlock->size))
    {
        totalSize = size;
        umm blockSize = maximum(totalSize, MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE);
        PlatformMemoryBlock *nextBlock = gMemoryApi->allocate_memory(blockSize, flags);
        nextBlock->prev = arena->currentBlock;
        arena->currentBlock = nextBlock;
    }

    i_expect(arena->currentBlock && ((arena->currentBlock->used + totalSize) <= arena->currentBlock->size));

    umm alignOffset = get_alignment_offset(arena, alignment);
    umm offsetInBlock = arena->currentBlock->used + alignOffset;
    result = arena->currentBlock->base + offsetInBlock;
    arena->currentBlock->used += totalSize;

    i_expect(totalSize >= size);
    i_expect(arena->currentBlock->used <= arena->currentBlock->size);

    if (!(flags & Memory_NoClear))
    {
        copy_single(totalSize, 0x0, result);
    }

    return result;
}

internal ALLOCATE_MEMORY_COPY(arena_allocate_copy)
{
    flags |= Memory_NoClear;
    void *result = arena_allocate_size(allocator, size, flags);
    copy(size, source, result);
    return result;
}

internal ALLOCATE_MEMORY_STRINGZ(arena_allocate_stringz)
{
    String result;
    result.size = 0;
    result.data = (u8 *)arena_allocate_size(allocator, source.size + 1, align_memory_alloc(1, false));
    if (result.data)
    {
        result.size = source.size;
        copy(source.size, source.data, result.data);
        result.data[result.size] = 0;
    }
    return result;
}

internal BOOTSTRAP_ALLOCATOR(arena_bootstrap)
{
    MemoryArena bootstrapArena = {};
    void *result = arena_allocate_size(&bootstrapArena, totalSize, flags);
    *(MemoryArena *)((u8 *)result + offsetToAlloc) = bootstrapArena;
    return result;
}

internal void
free_last_block(MemoryArena *arena)
{
    i_expect(arena->currentBlock);
    PlatformMemoryBlock *free = arena->currentBlock;
    arena->currentBlock = free->prev;
    gMemoryApi->deallocate_memory(free);
}

internal DEALLOCATE_ALL(arena_deallocate_all)
{
    MemoryArena *arena = (MemoryArena *)allocator;
    while (arena->currentBlock)
    {
        b32 isLastBlock = (arena->currentBlock->prev == 0);
        free_last_block(arena);
        if (isLastBlock)
        {
            // NOTE(michiel): If this arena is allocated on the previous block we could try to
            // derefence it in the next while check, so we break out early.
            break;
        }
    }
}

internal void
clear_arena(MemoryArena *arena)
{
    // NOTE(michiel): Deallocate all but the last block. Useful for a constant base block of some size.
    if (arena->currentBlock)
    {
        b32 isLastBlock = (arena->currentBlock->prev == 0);
        while (!isLastBlock)
        {
            free_last_block(arena);
            isLastBlock = (arena->currentBlock->prev == 0);
        }
        arena->currentBlock->used = 0;
    }
}

internal TempArenaMemory
begin_temporary_memory(MemoryArena *arena)
{
    TempArenaMemory result;
    result.arena = arena;
    result.block = arena->currentBlock;
    result.used  = arena->currentBlock ? arena->currentBlock->used : 0;
    ++arena->tempCount;
    return result;
}

internal void
end_temporary_memory(TempArenaMemory tempMem)
{
    MemoryArena *arena = tempMem.arena;
    while (arena->currentBlock != tempMem.block)
    {
        free_last_block(arena);
    }

    if (arena->currentBlock)
    {
        i_expect(arena->currentBlock->used >= tempMem.used);
        arena->currentBlock->used = tempMem.used;
    }

    i_expect(arena->tempCount > 0);
    --arena->tempCount;
}

internal void
keep_temporary_memory(TempArenaMemory tempMem)
{
    MemoryArena *arena = tempMem.arena;
    i_expect(arena->tempCount > 0);
    --arena->tempCount;
}

internal void
check_arena_integrity(MemoryArena *arena)
{
    i_expect(arena->tempCount == 0);
}

internal INIT_ALLOCATOR(initialize_arena_allocator)
{
    dest->allocator        = allocator;
    dest->bootstrap_alloc  = arena_bootstrap;
    dest->allocate_size    = arena_allocate_size;
    dest->allocate_copy    = arena_allocate_copy;
    dest->allocate_stringz = arena_allocate_stringz;
    dest->reallocate_size  = 0;
    dest->deallocate       = 0;
    dest->deallocate_all   = arena_deallocate_all;
}

//
// NOTE(michiel): Stretchy buf, supported by a platform allocator (so a minimum of MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE)
//

internal void
mbuf_grow_(void **bufferAddress, umm newCount, umm elemSize)
{
    void *buf = *bufferAddress;
    i_expect(mbuf_magic(buf) == MEMORY_BUF_MAGIC);
    i_expect(mbuf_max_count(buf) <= (U64_MAX - 1) / 2);

    umm newTotalCount = mbuf_count(buf) + newCount;
    umm newMaxCount = 2 * mbuf_max_count(buf);

    if (newTotalCount < (MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE / elemSize))
    {
        newTotalCount = (MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE / elemSize);
    }
    if (newTotalCount < newMaxCount)
    {
        newTotalCount = newMaxCount;
    }

    i_expect(newTotalCount >= (mbuf_count(buf) + newCount));
    i_expect(newTotalCount <= (U64_MAX - offset_of(MemoryBuffer, data)) / elemSize);
    umm newSize = offset_of(MemoryBuffer, data) + newTotalCount * elemSize;

    PlatformMemoryBlock *block = 0;
    if (buf)
    {
        block = mbuf_hdr(buf)->block;
    }
    PlatformMemoryBlock *newBlock = gMemoryApi->reallocate_memory(block, newSize, default_memory_alloc());
    i_expect(newBlock && newBlock->base);

    MemoryBuffer *memBuf = (MemoryBuffer *)newBlock->base;

    if (!buf)
    {
        memBuf->magic = MEMORY_BUF_MAGIC;
        memBuf->count = 0;
    }

    i_expect(memBuf->magic == MEMORY_BUF_MAGIC);

    memBuf->block = newBlock;
    memBuf->maxCount = newTotalCount;

    *bufferAddress = memBuf->data;
}

internal void
mbuf_printf_(void **bufferAddress, char *fmt, ...)
{
    void *buf = *bufferAddress;
    i_expect(mbuf_magic(buf) == MEMORY_BUF_MAGIC);
    char *bufEnd = (char *)mbuf_end((char *)buf);
    va_list args;
    va_start(args, fmt);
    umm maxCount = mbuf_max_count(buf) - mbuf_count(buf);
    umm end = 1 + vsnprintf(bufEnd, maxCount, fmt, args);
    va_end(args);

    if (end > maxCount)
    {
        mbuf_grow_(&buf, end + mbuf_count(buf), 1);
        va_start(args, fmt);
        umm newMaxCnt = mbuf_max_count(buf) - mbuf_count(buf);
        bufEnd = (char *)mbuf_end((char *)buf);
        end = 1 + vsnprintf(bufEnd, newMaxCnt, fmt, args);
        va_end(args);
        i_expect(end <= newMaxCnt);
    }
    mbuf_hdr(buf)->count += end - 1;

    *bufferAddress = buf;
}

//
// NOTE(michiel): Map, supported by a platform allocator (so a minimum of MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE)
//

internal void
mmap_deallocate(MemoryMap *map)
{
    gMemoryApi->deallocate_memory(map->block);
    map->block = 0;
    map->keys = 0;
    map->values = 0;
    map->count = 0;
    map->maxCount = 0;
}

internal void
mmap_grow(MemoryMap *map, umm newMaxCount)
{
    newMaxCount = maximum(newMaxCount, MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE / sizeof(u64));

    i_expect((4 * map->count) < (3 * newMaxCount));
    i_expect(is_pow2(newMaxCount));

    umm newSize = 2 * newMaxCount * sizeof(u64);

    PlatformMemoryBlock *newBlock = gMemoryApi->allocate_memory(newSize, no_clear_memory_alloc());

    MemoryMap newMap = {};
    newMap.block = newBlock;
    newMap.maxCount = newMaxCount;
    newMap.keys = (u64 *)newBlock->base;
    newMap.values = (u64 *)newBlock->base + newMaxCount;
    copy_single(newMaxCount * sizeof(u64), 0xFF, newMap.keys);

    for (u32 mapIndex = 0; mapIndex < map->maxCount; ++mapIndex)
    {
        if (map->keys[mapIndex] < MEMORY_MAP_DELETED_KEY)
        {
            mmap_u64_put_u64(&newMap, map->keys[mapIndex], map->values[mapIndex]);
        }
    }

    mmap_deallocate(map);
    *map = newMap;
}

internal b32
mmap_u64_has_key_u64(MemoryMap *map, u64 key)
{
    b32 result = false;
    if (map->count)
    {
        i_expect(is_pow2(map->maxCount));
        i_expect(map->count < map->maxCount);

        umm hash = (umm)hash_bytes(&key, sizeof(u64));
        while (true)
        {
            hash &= map->maxCount - 1;
            if (map->keys[hash] == key)
            {
                result = true;
                break;
            }
            else if (map->keys[hash] == MEMORY_MAP_EMPTY_KEY)
            {
                break;
            }
            ++hash;
        }
    }
    return result;
}

internal u64
mmap_u64_get_u64(MemoryMap *map, u64 key)
{
    i_expect(key != MEMORY_MAP_EMPTY_KEY);
    i_expect(key != MEMORY_MAP_DELETED_KEY);

    u64 result = 0;
    if (map->count)
    {
        i_expect(is_pow2(map->maxCount));
        i_expect(map->count < map->maxCount);

        umm hash = (umm)hash_bytes(&key, sizeof(u64));
        while (true)
        {
            hash &= map->maxCount - 1;
            if (map->keys[hash] == key)
            {
                result = map->values[hash];
                break;
            }
            else if (map->keys[hash] == MEMORY_MAP_EMPTY_KEY)
            {
                break;
            }
            ++hash;
        }
    }
    return result;
}

internal void
mmap_u64_put_u64(MemoryMap *map, u64 key, u64 value)
{
    i_expect(key != MEMORY_MAP_EMPTY_KEY);
    i_expect(key != MEMORY_MAP_DELETED_KEY);

    if ((4 * map->count) >= (3 * map->maxCount))
    {
        // NOTE(michiel): There could be a loop, buf we know that when we call this function from mmap_grow we already
        // have enough space.
        mmap_grow(map, 2 * map->maxCount);
    }

    i_expect((4 * map->count) < (3 * map->maxCount));
    i_expect(is_pow2(map->maxCount));

    umm hash = (umm)hash_bytes(&key, sizeof(u64));
    while (true)
    {
        hash &= map->maxCount - 1;
        if ((map->keys[hash] == MEMORY_MAP_EMPTY_KEY) ||
            (map->keys[hash] == MEMORY_MAP_DELETED_KEY))
        {
            ++map->count;
            map->keys[hash] = key;
            map->values[hash] = value;
            break;
        }
        else if (map->keys[hash] == key)
        {
            map->values[hash] = value;
        }
        ++hash;
    }
}

// NOTE(michiel): No need for the name, but it is nice and consistent in the macros
internal void
mmap_u64_rem_u64(MemoryMap *map, u64 key)
{
    i_expect(key != MEMORY_MAP_EMPTY_KEY);
    i_expect(key != MEMORY_MAP_DELETED_KEY);

    if (map->count)
    {
        i_expect(is_pow2(map->maxCount));
        i_expect(map->count < map->maxCount);

        umm hash = (umm)hash_bytes(&key, sizeof(u64));
        while (true)
        {
            hash &= map->maxCount - 1;
            if (map->keys[hash] == key)
            {
                --map->count;
                map->keys[hash] = MEMORY_MAP_DELETED_KEY;
                break;
            }
            else if (map->keys[hash] == MEMORY_MAP_EMPTY_KEY)
            {
                break;
            }
            ++hash;
        }
    }
}

//
// NOTE(michiel): Memory string interning
//
internal void
minterns_deallocate(MemoryInterns *interns)
{
    arena_deallocate_all(&interns->arena);
    mmap_deallocate(&interns->hashMap);
}

internal String
minterned_string(MemoryInterns *interns, String str)
{
    String result = {};
    if (str.size)
    {
        u64 hash = hash_bytes(str.data, str.size);
        u64 key  = hash ? hash : 1;
        InternedString *intern = (InternedString *)mmap_u64_get(&interns->hashMap, key);
        InternedString *it = intern;

        while (it)
        {
            String itStr = string(it->size, it->data);
            if (itStr == str)
            {
                result = itStr;
                break;
            }
            it = it->next;
        }

        if (!result.size)
        {
            umm newSize = offset_of(InternedString, data) + str.size + 1;
            InternedString *newIntern = (InternedString *)arena_allocate_size(&interns->arena, newSize, align_memory_alloc(1, false));
            newIntern->next = intern;
            newIntern->size = str.size;
            copy(str.size, str.data, newIntern->data);
            newIntern->data[str.size] = 0;
            mmap_u64_put(&interns->hashMap, key, newIntern);
            result = string(newIntern->size, newIntern->data);
        }
    }

    return result;
}

internal String
minterned_string(MemoryInterns *interns, char *str)
{
    return minterned_string(interns, string(str));
}

internal String
minterned_string_fmt(MemoryInterns *interns, umm maxTempData, char *tempData, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    umm total = vsnprintf(tempData, maxTempData - 1, fmt, args);
    va_end(args);

    if (total > (maxTempData - 1))
    {
        // TODO(michiel): Notify user
        total = maxTempData - 1;
    }
    tempData[total] = 0;
    return minterned_string(interns, string(total, tempData));
}
