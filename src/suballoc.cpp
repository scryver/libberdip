#define MIN_SUBALLOC_BITS  4
#define MIN_SUBALLOC_SIZE  (1 << MIN_SUBALLOC_BITS)
compile_expect(MIN_SUBALLOC_SIZE >= sizeof(BucketList));

#define SUBALLOC_HEADER_OFFSET sizeof(umm)

#if SUB_ALLOC_DEBUG
// NOTE(michiel): Use this expect to test every single part of the allocator
#define suballoc_expect(...) i_expect(__VA_ARGS__)
#else
#define suballoc_expect(...)
#endif

internal u32
bucket_from_size(SubAllocator *allocator, u32 size)
{
    suballoc_expect(is_pow2(size));

    BitScanResult highBit = find_most_significant_set_bit(size);
    suballoc_expect(highBit.found);

    u32 bucket = allocator->bucketCount - 1 + MIN_SUBALLOC_BITS - highBit.index;
    return bucket;
}

internal u32
bucket_from_request(SubAllocator *allocator, u32 request)
{
    BitScanResult highBit = find_most_significant_set_bit(request);
    suballoc_expect(highBit.found);

    u32 size = (1 << highBit.index);
    if (request > size) {
        size <<= 1;
    }

    u32 bucket = bucket_from_size(allocator, size);
    return bucket;
}

internal u32
get_bucket_size(SubAllocator *allocator, u32 bucket)
{
    u32 result = 1 << (allocator->bucketCount - 1 + MIN_SUBALLOC_BITS - bucket);
    suballoc_expect(bucket_from_size(allocator, result) == bucket);
    return result;
}

#if SUB_ALLOC_DEBUG
internal b32
update_mark(SubAllocator *allocator, u8 *newValue)
{
    b32 result = true;
    if (newValue > allocator->mark)
    {
        if (newValue <= allocator->end)
        {
            allocator->mark = newValue;
        }
        else
        {
            result = false;
        }
    }
    return result;
}
#endif

internal void
add_to_free_list(SubAllocator *allocator, BucketList *entry, u32 bucket)
{
    entry->next = allocator->freeLists[bucket];
    allocator->freeLists[bucket] = entry;
}

internal BucketList *
remove_first_free(SubAllocator *allocator, u32 bucket)
{
    BucketList *result = allocator->freeLists[bucket];

    if (result) {
        allocator->freeLists[bucket] = result->next;
        result->next = 0;
    }

    return result;
}

//#if SUB_ALLOC_DEBUG
internal b32
is_right(SubAllocator *allocator, BucketList *entry, u32 size)
{
    suballoc_expect(is_pow2(size));
    umm offset = ((u8 *)entry - allocator->base);
    return offset & size;
}
//#endif

internal BucketList *
get_sibling(SubAllocator *allocator, BucketList *entry, u32 size)
{
    suballoc_expect(is_pow2(size));
    BucketList *result = (BucketList *)(allocator->base + (((u8 *)entry - allocator->base) ^ size));
    return result;
}

internal void
coalesce_lists(SubAllocator *allocator)
{
    for (u32 bucketIdx = allocator->bucketCount - 1; bucketIdx > 0; --bucketIdx)
    {
        BucketList *entry = allocator->freeLists[bucketIdx];
        u32 bucketSize = get_bucket_size(allocator, bucketIdx);

        BucketList *prevEntry = 0;
        while (entry)
        {
            BucketList *sibling = get_sibling(allocator, entry, bucketSize);
            BucketList *nextEntry = entry->next;
            BucketList *prevSib = 0;
            BucketList *nextSib = 0;
            b32 found = false;

            if (!sibling->isUsed) {
                for (BucketList *test = entry->next;
                     test;
                     test = test->next)
                {
                    if (test == sibling) {
                        nextSib = test->next;
                        found = true;
                        break;
                    }
                    prevSib = test;
                }
            }

            if (found) {
                if (nextEntry == sibling) {
                    i_expect(!prevSib);
                    nextEntry = nextSib;
                }
                else if (prevSib) {
                    prevSib->next = nextSib;
                }

                if (prevEntry) {
                    prevEntry->next = nextEntry;
                } else {
                    allocator->freeLists[bucketIdx] = nextEntry;
                }

                entry->next = 0;
                add_to_free_list(allocator, (entry < sibling) ? entry : sibling, bucketIdx - 1);
            } else {
                prevEntry = entry;
            }
            entry = nextEntry;
        }
    }
}

internal b32
init_sub_allocator(SubAllocator *allocator, u32 size, u8 *data)
{
    i_expect((size > (MIN_SUBALLOC_SIZE * 16)) && "Use more than 16x the minimum allocation when using this");
    i_expect((size <= S32_MAX) && "We only support up to 2GB as memory arena");
    i_expect(is_pow2(size) && "Only powers of two are allowed as size");
    i_expect((((umm)data & ~0xF) == (umm)data) && "The data must be 16 bytes aligned");

    b32 result = false;

    allocator->totalSize = 0;

    BitScanResult highBit = find_most_significant_set_bit(size);
    if (highBit.found)
    {
        allocator->totalSize = size;

        allocator->base = data;
        allocator->end = allocator->base + size;

#if SUB_ALLOC_DEBUG
        allocator->mark = allocator->base;
        update_mark(allocator, allocator->base + sizeof(BucketList));
#endif

        u32 bucketCount = (highBit.index - MIN_SUBALLOC_BITS + 1);
        i_expect(bucketCount < array_count(allocator->freeLists));
        allocator->bucketCount = bucketCount;

#if 0
        for (u32 bucketIdx = 0; bucketIdx < bucketCount; ++bucketIdx)
        {
            BucketList *list = allocator->freeLists + bucketIdx;
            list->prev = list->next = list;
        }
#endif

        BucketList *baseBucket = (BucketList *)allocator->base;
        add_to_free_list(allocator, baseBucket, 0);

        result = true;
    }

    return result;
}

internal void *
sub_alloc(SubAllocator *allocator, u32 requestSize)
{
    i_expect(allocator);
    i_expect(allocator->base);

    void *result = 0;
    u32 totalSize = requestSize + SUBALLOC_HEADER_OFFSET;

    if ((requestSize > 0) && (totalSize <= allocator->totalSize))
    {
        u32 bucket = bucket_from_request(allocator, totalSize);
        u32 origBucket = bucket;
        u32 bucketSize = get_bucket_size(allocator, bucket);

        b32 doSplit = false;
        BucketList *allocPtr = remove_first_free(allocator, bucket);
        while (!allocPtr && bucket) {
            allocPtr = remove_first_free(allocator, --bucket);
            bucketSize <<= 1;
            doSplit = true;
        }

        if (!allocPtr) {
            coalesce_lists(allocator);

            bucket = origBucket;
            bucketSize = get_bucket_size(allocator, bucket);

            doSplit = false;
            allocPtr = remove_first_free(allocator, bucket);
            while (!allocPtr && bucket) {
                allocPtr = remove_first_free(allocator, --bucket);
                bucketSize <<= 1;
                doSplit = true;
            }
        }

        if (allocPtr)
        {
            if (doSplit)
            {
                ++bucket;
                bucketSize >>= 1;
                while (bucket <= origBucket)
                {
                    BucketList *sibling = (BucketList *)((u8 *)allocPtr + bucketSize);
                    suballoc_expect(!sibling->isUsed);
                    suballoc_expect(is_right(allocator, sibling, bucketSize));
                    add_to_free_list(allocator, sibling, bucket);

                    ++bucket;
                    bucketSize >>= 1;
                }
                suballoc_expect(get_bucket_size(allocator, origBucket) == (bucketSize << 1));
            }
            u32 size = get_bucket_size(allocator, origBucket);
            allocPtr->size = size;
            suballoc_expect((((u8 *)allocPtr - allocator->base) & (size - 1)) == 0);
            allocPtr->isUsed = true;

#if SUB_ALLOC_DEBUG
            update_mark(allocator, (u8 *)allocPtr + size);
#endif

            result = (u8 *)allocPtr + SUBALLOC_HEADER_OFFSET;
        }
    }

    return result;
}

internal void *
sub_dealloc(SubAllocator *allocator, void *pointer)
{
    void *result = pointer;

    if (pointer)
    {
        BucketList *entry = (BucketList *)((u8 *)pointer - SUBALLOC_HEADER_OFFSET);
        suballoc_expect(entry->isUsed);
#if SUB_ALLOC_DEBUG
        suballoc_expect((u8 *)entry < allocator->mark);
#endif
        entry->isUsed = false;
        suballoc_expect((entry->size & 0x7) == 0);
        suballoc_expect(entry->size <= S32_MAX);
        suballoc_expect(is_pow2(entry->size));

        u32 bucket = bucket_from_size(allocator, entry->size);
        add_to_free_list(allocator, entry, bucket);

        result = 0;
    }

    return result;
}

