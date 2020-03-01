enum MemoryAllocFlags
{
    Memory_NoClear     = 0x01,
    Memory_Debug       = 0x0F,
};

struct MemoryAllocInfo
{
    u32 flags;
    u32 alignSize;
};

internal MemoryAllocInfo
default_memory_alloc(void)
{
    MemoryAllocInfo result = {};
    result.alignSize = 4;
}

internal MemoryAllocInfo
no_clear_memory_alloc(void)
{
    MemoryAllocInfo result = default_memory_alloc();
    result.flags |= Memory_NoClear;
}

internal MemoryAllocInfo
debug_memory_alloc(void)
{
    MemoryAllocInfo result = default_memory_alloc();
    result.flags |= Memory_Debug;
}

internal MemoryAllocInfo
align_memory_alloc(u32 alignment)
{
    i_expect(is_pow2(alignment));
    MemoryAllocInfo result = default_memory_alloc();
    result.alignSize = alignment;
}

#define ALLOCATE_MEMORY_SIZE(name)    void *name(void *allocator, umm size, MemoryAllocInfo allocInfo)
typedef ALLOCATE_MEMORY_SIZE(AllocateMemorySize);

#define REALLOCATE_MEMORY_SIZE(name)  void *name(void *allocator, void *memory, umm size, MemoryAllocInfo allocInfo)
typedef REALLOCATE_MEMORY_SIZE(ReallocateMemorySize);

#define DEALLOCATE_MEMORY_SIZE(name)  void *name(void *allocator, void *memory, umm size)
typedef DEALLOCATE_MEMORY_SIZE(DeallocateMemorySize);

