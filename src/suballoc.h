#define SUB_ALLOC_DEBUG 0

union BucketList
{
    union BucketList *next;
    umm size;
    umm isUsed : 1;
};

// NOTE(michiel): We don't support more than 2GB as totalSize so this should be _fine_
#define MAX_SUB_ALLOC_BUCKETS 32
struct SubAllocator
{
    u8 *base;
#if SUB_ALLOC_DEBUG
    u8 *mark;
#endif
    u8 *end;
    u32 totalSize;
    u32 bucketCount;
    BucketList *freeLists[MAX_SUB_ALLOC_BUCKETS];
};

internal b32 init_sub_allocator(SubAllocator *allocator, u32 size, u8 *data);
internal void *sub_alloc(SubAllocator *allocator, u32 requestSize);
internal void *sub_dealloc(SubAllocator *allocator, void *pointer);
