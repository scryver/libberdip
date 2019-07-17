#define SUB_ALLOC_DEBUG 0

union SubAllocItem
{
    union SubAllocItem *next; // NOTE(michiel): In the free list it is used to point to the next free item
    umm size;                 // NOTE(michiel): If given to the user as memory, this will have the bucket size
    umm isUsed : 1;           // NOTE(michiel): In both cases the lowest bit will always be zero so we use it
    // to distinguish between the two.
};

// NOTE(michiel): We don't support more than 2GB as totalSize so this should be _fine_
#define MAX_SUB_ALLOC_BUCKETS 32
struct SubAllocator
{
    u8 *base;          // NOTE(michiel): Base of used memory pool
#if SUB_ALLOC_DEBUG
    u8 *mark;          // NOTE(michiel): Used up to this mark point of memory
    u32 splitCount;    // NOTE(michiel): Count of blocks that got split to provide memory
    u32 coalesceCount; // NOTE(michiel): Count of blocks that got coalesced (should be times 2, 2 blocks go to 1)
#endif
    u8 *end;           // NOTE(michiel): One byte past the last accessible memory position
    u32 totalSize;     // NOTE(michiel): Total usable size
    u32 bucketCount;   // NOTE(michiel): Buckets that are in use
    SubAllocItem *freeLists[MAX_SUB_ALLOC_BUCKETS]; // NOTE(michiel): The freelists
};

// NOTE(michiel): Provide the sub-allocator with some already allocated memory,
// it will then subdivide this memory among requests as necessary.
internal b32 init_sub_allocator(SubAllocator *allocator, u32 size, u8 *data);
// NOTE(michiel): Allocate some amount of memory. Will try to coalesce blocks if it
// can't provide memory. If it still can't provide memory, it will return a null pointer.
internal void *sub_alloc(SubAllocator *allocator, u32 requestSize);
// NOTE(michiel): Deallocate memory, allocated with this same interface. It will always
// return 0, so you can use it to immediatly clear the pointer after this function call.
internal void *sub_dealloc(SubAllocator *allocator, void *pointer);
// NOTE(michiel): This function is _mostly_ for internal use. But if you know when you want
// to spend time to coalesce blocks, that go ahead and start this function. It will operate
// on the free-lists and tries to coalesce as much as possible (small to large blocks).
internal u32 sub_coalesce(SubAllocator *allocator);
