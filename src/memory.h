#ifndef MEMORY_DEFAULT_ALIGN
#define MEMORY_DEFAULT_ALIGN 4
#endif

#ifndef MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE
#define MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE megabytes(1)
#endif
compile_expect(MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE <= gigabytes(2));

enum MemoryAllocFlags
{
    Memory_AlignMask   = 0x0000FFFF,
    Memory_NoClear     = 0x01000000,
    Memory_Debug       = 0xFF000000,
};
compile_expect((MEMORY_DEFAULT_ALIGN & Memory_AlignMask) == MEMORY_DEFAULT_ALIGN);

struct PlatformMemoryBlock
{
    umm size;
    umm used;
    u8 *base;
    PlatformMemoryBlock *prev;
};

// NOTE(michiel):
// Arenas are for data that needs to persist for ever, the last allocations can be freed by using the temp memory interface
// SubAlloc is used in conjunction with the arena (or similar) it is passed in memory to be subdivided in smaller pieces
// MemoryBuffer and MemoryMap are a growable array and map for data without a known upper bound (uses the platform layer directly)
// MemoryInterns is used as constant string pool, no frees only adds. Useful when comparing a lot of strings
// (it boils down to a pointer comparison when using 2 interned strings).

struct MemoryAPI;
#define INIT_MEMORY_API(name) void name(MemoryAPI *memoryApi)
typedef INIT_MEMORY_API(InitMemoryAPI);

// TODO(michiel): Platform flags for over/underflow of data
#define PLATFORM_ALLOCATE_MEMORY(name)         PlatformMemoryBlock *name(umm size, u32 flags)
typedef PLATFORM_ALLOCATE_MEMORY(PlatformAllocateMemory);

// NOTE(michiel): Can be used to allocate by passing a 0-pointer as block
#define PLATFORM_REALLOCATE_MEMORY(name)       PlatformMemoryBlock *name(PlatformMemoryBlock *block, umm newSize, u32 flags)
typedef PLATFORM_REALLOCATE_MEMORY(PlatformReallocateMemory);

// NOTE(michiel): Returns 0 on success, otherwise the old pointer
#define PLATFORM_DEALLOCATE_MEMORY(name)       void *name(PlatformMemoryBlock *block)
typedef PLATFORM_DEALLOCATE_MEMORY(PlatformDeallocateMemory);

// NOTE(michiel): Very destructive
#define PLATFORM_DEALLOCATE_ALL_MEMORY(name)   void name(void)
typedef PLATFORM_DEALLOCATE_ALL_MEMORY(PlatformDeallocateAllMemory);

//
// NOTE(michiel): Only allocate_memory_size needs to be defined (others can be 0)
//

struct MemoryAllocator;
#define INIT_ALLOCATOR(name)          void name(void *allocator, MemoryAllocator *dest)
typedef INIT_ALLOCATOR(InitAllocator);

#define ALLOCATE_MEMORY_SIZE(name)    void *name(void *allocator, umm size, u32 flags)
typedef ALLOCATE_MEMORY_SIZE(AllocateMemorySize);

#define ALLOCATE_MEMORY_COPY(name)    void *name(void *allocator, umm size, void *source, u32 flags)
typedef ALLOCATE_MEMORY_COPY(AllocateMemoryCopy);

#define ALLOCATE_MEMORY_STRINGZ(name) String name(void *allocator, String source)
typedef ALLOCATE_MEMORY_STRINGZ(AllocateMemoryStringZ);

#define REALLOCATE_MEMORY_SIZE(name)  void *name(void *allocator, umm size, void *memory, u32 flags)
typedef REALLOCATE_MEMORY_SIZE(ReallocateMemorySize);

#define DEALLOCATE_MEMORY(name)       void *name(void *allocator, void *memory)
typedef DEALLOCATE_MEMORY(DeallocateMemory);

#define DEALLOCATE_ALL(name)          void  name(void *allocator)
typedef DEALLOCATE_ALL(DeallocateAll);

// NOTE(michiel): Used to allocate a struct with a member which is the allocator to use to allocate the struct.
// This member can't be a pointer!
#define BOOTSTRAP_ALLOCATOR(name)     void *name(umm totalSize, umm offsetToAlloc, u32 flags)
typedef BOOTSTRAP_ALLOCATOR(BootstrapAllocator);

struct MemoryAPI
{
    PlatformAllocateMemory *allocate_memory;
    PlatformReallocateMemory *reallocate_memory;
    PlatformDeallocateMemory *deallocate_memory;
    PlatformDeallocateAllMemory *deallocate_all;
};

struct MemoryAllocator
{
    void *allocator;
    BootstrapAllocator    *bootstrap_alloc;
    AllocateMemorySize    *allocate_size;
    AllocateMemoryCopy    *allocate_copy;
    AllocateMemoryStringZ *allocate_stringz;
    ReallocateMemorySize  *reallocate_size;
    DeallocateMemory      *deallocate;
    DeallocateAll         *deallocate_all;
};

internal void *
allocate_size(MemoryAllocator *alloc, umm size, u32 flags)
{
    i_expect(alloc->allocate_size);
    return alloc->allocate_size(alloc->allocator, size, flags);
}

internal void *
allocate_copy(MemoryAllocator *alloc, umm size, void *source, u32 flags)
{
    i_expect(alloc->allocate_copy);
    return alloc->allocate_copy(alloc->allocator, size, source, flags);
}

internal String
allocate_stringz(MemoryAllocator *alloc, String source)
{
    i_expect(alloc->allocate_stringz);
    return alloc->allocate_stringz(alloc->allocator, source);
}

internal void *
reallocate_size(MemoryAllocator *alloc, umm size, void *source, u32 flags)
{
    i_expect(alloc->reallocate_size);
    return alloc->reallocate_size(alloc->allocator, size, source, flags);
}

internal void *
deallocate(MemoryAllocator *alloc, void *memory)
{
    i_expect(alloc->deallocate);
    return alloc->deallocate(alloc->allocator, memory);
}

internal void
deallocate_all(MemoryAllocator *alloc)
{
    i_expect(alloc->deallocate_all);
    alloc->deallocate_all(alloc->allocator);
}

#define allocate_struct(a, t, i)            (t *)a->allocate_size(a->allocator, sizeof(t), i)
#define allocate_array(a, t, c, i)          (t *)a->allocate_size(a->allocator, sizeof(t)*c, i)
#define allocate_copy_struct(a, t, s, i)    (t *)a->allocate_copy(a->allocator, s, sizeof(t), i)
#define allocate_copy_array(a, t, c, s, i)  (t *)a->allocate_copy(a->allocator, s, sizeof(t)*c, i)

//
//
//

internal u32
default_memory_alloc(void)
{
    u32 result = MEMORY_DEFAULT_ALIGN;
    return result;
}

internal u32
no_clear_memory_alloc(void)
{
    u32 result = default_memory_alloc();
    result |= Memory_NoClear;
    return result;
}

internal u32
debug_memory_alloc(void)
{
    u32 result = default_memory_alloc();
    result |= Memory_Debug;
    return result;
}

internal u32
align_memory_alloc(u32 alignment, b32 clear = true)
{
    i_expect(is_pow2(alignment));
    i_expect(alignment < Memory_AlignMask);
    u32 result = alignment;
    if (clear) {
        result &= ~Memory_NoClear;
    } else {
        result |= Memory_NoClear;
    }
    return result;
}

//
// NOTE(michiel): Arenas
//

struct MemoryArena
{
    PlatformMemoryBlock *currentBlock;
    s32 tempCount;
};

struct TempArenaMemory
{
    MemoryArena *arena;
    PlatformMemoryBlock *block;
    umm used;
};

//
// NOTE(michiel): Stretchy buf, supported by a platform allocator (so a minimum of MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE)
//
#define MEMORY_BUF_MAGIC 0xB0FFE20123456789
struct MemoryBuffer
{
    // TODO(michiel): Piggy header
    u64 magic;
    PlatformMemoryBlock *block;
    umm maxCount;
    umm count;
    u8 data[1];
};

// NOTE(michiel): Internal use
#define mbuf_hdr(ptr)              ((MemoryBuffer *)((u8 *)(ptr) - offset_of(MemoryBuffer, data)))
#define mbuf_grow(ptr, n)          (mbuf_grow_((void **)&(ptr), (n), sizeof(*(ptr))))
// NOTE(michiel): Supporting functions
#define mbuf_magic(ptr)            ((ptr) ? mbuf_hdr(ptr)->magic : MEMORY_BUF_MAGIC)
#define mbuf_count(ptr)            ((ptr) ? mbuf_hdr(ptr)->count : 0)
#define mbuf_max_count(ptr)        ((ptr) ? mbuf_hdr(ptr)->maxCount : 0)
#define mbuf_end(ptr)              ((ptr) + mbuf_count(ptr))
#define mbuf_last(ptr)             ((ptr) ? (ptr) + mbuf_count(ptr) - 1 : 0)
#define mbuf_sizeof(ptr)           ((ptr) ? buf_len(ptr) * sizeof(*(ptr)) : 0)
// NOTE(michiel): Interface
#define mbuf_deallocate(ptr)       ((ptr) ? (gMemoryApi.deallocate_memory(mbuf_hdr(ptr)->block), (ptr) = 0) : 0)
#define mbuf_fit(ptr, n)           ((n) <= mbuf_max_count(ptr) ? 0 : (mbuf_grow((ptr), (n)), 0))
#define mbuf_push(ptr, v)          (mbuf_fit((ptr), mbuf_count(ptr) + 1), (ptr)[mbuf_hdr(ptr)->count++] = (v))
#define mbuf_pop(ptr)              (mbuf_count(ptr) ? (ptr)[--mbuf_hdr(ptr)->count] : *ptr) // NOTE(michiel): Fails on invalid access
#define mbuf_clear(ptr)            ((ptr) ? mbuf_hdr(ptr)->count = 0 : 0)
// NOTE(michiel): Only use with byte buffers (elemsize == 1)
#define mbuf_printf(ptr, fmt, ...) (mbuf_printf_((void **)&(ptr), (fmt), ## __VA_ARGS__))

//
// NOTE(michiel): Map, supported by a platform allocator (so a minimum of MEMORY_MINIMUM_PLATFORM_BLOCK_SIZE)
//
struct MemoryMap // TODO(michiel): Need a better name
{
    PlatformMemoryBlock *block;
    umm maxCount;
    umm count;
    u64 *keys;
    u64 *values;
};

#define MEMORY_MAP_EMPTY_KEY    0xFFFFFFFFFFFFFFFFULL
#define MEMORY_MAP_DELETED_KEY  0xFFFFFFFFFFFFFFFEULL

internal u64  mmap_u64_get_u64(MemoryMap *map, u64 key);
internal void mmap_u64_put_u64(MemoryMap *map, u64 key, u64 value);
internal void mmap_u64_rem_u64(MemoryMap *map, u64 key);

// NOTE(michiel): Pointer keys and values
#define mmap_get(m, k)          ptr_from_u64(mmap_u64_get_u64((m), u64_from_ptr(k)))
#define mmap_put(m, k, v)       mmap_u64_put_u64((m), u64_from_ptr(k), u64_from_ptr(v))
#define mmap_remove(m, k)       mmap_u64_rem_u64((m), u64_from_ptr(k))
// NOTE(michiel): Pointer keys, u64 values
#define mmap_get_u64(m, k)      mmap_u64_get_u64((m), u64_from_ptr(k))
#define mmap_put_u64(m, k, v)   mmap_u64_put_u64((m), u64_from_ptr(k), (v))
#define mmap_remove_u64(m, k)   mmap_u64_rem_u64((m), u64_from_ptr(k))
// NOTE(michiel): u64 keys, pointer values
#define mmap_u64_get(m, k)      ptr_from_u64(mmap_u64_get_u64((m), (k)))
#define mmap_u64_put(m, k, v)   mmap_u64_put_u64((m), (k), u64_from_ptr(v))
#define mmap_u64_remove(m, k)   mmap_u64_rem_u64((m), (k))

//
// NOTE(michiel): Memory string interning
//
struct MemoryInterns
{
    MemoryArena arena;
    MemoryMap   hashMap;
};

struct InternedString
{
    InternedString *next;
    u32 size;
    char data[1];
};
