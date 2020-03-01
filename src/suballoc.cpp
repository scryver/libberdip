#define MIN_SUBALLOC_SIZE  (1 << MIN_SUBALLOC_BITS)
compile_expect(MIN_SUBALLOC_SIZE >= sizeof(SubAllocItem));

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
add_to_free_list(SubAllocator *allocator, SubAllocItem *entry, u32 bucket)
{
#if 0
    // TODO(michiel): Maybe sort the free list? Always return lowest memory first
    SubAllocItem *prev = 0;
    SubAllocItem *item = allocator->freeLists[bucket];
    while (item && (item < entry)) {
        prev = item;
        item = item->next;
    }
    entry->next = item;
    if (prev) {
        prev->next = entry;
    } else {
        allocator->freeLists[bucket] = entry;
    }
#else
    entry->next = allocator->freeLists[bucket];
    allocator->freeLists[bucket] = entry;
#endif
}

internal SubAllocItem *
remove_first_free(SubAllocator *allocator, u32 bucket)
{
    SubAllocItem *result = allocator->freeLists[bucket];

    if (result) {
        allocator->freeLists[bucket] = result->next;
        result->next = 0;
    }

    return result;
}

//#if SUB_ALLOC_DEBUG
internal b32
is_right(SubAllocator *allocator, SubAllocItem *entry, u32 size)
{
    suballoc_expect(is_pow2(size));
    umm offset = ((u8 *)entry - allocator->base);
    return offset & size;
}
//#endif

internal SubAllocItem *
get_sibling(SubAllocator *allocator, SubAllocItem *entry, u32 size)
{
    suballoc_expect(is_pow2(size));
    SubAllocItem *result = (SubAllocItem *)(allocator->base + (((u8 *)entry - allocator->base) ^ size));
    return result;
}

internal u32
sub_coalesce(SubAllocator *allocator)
{
    // NOTE(michiel): This returns the number of blocks that got coalesced.
    u32 result = 0;
    for (u32 bucketIdx = allocator->bucketCount - 1; bucketIdx > 0; --bucketIdx)
    {
        SubAllocItem *entry = allocator->freeLists[bucketIdx];
        u32 bucketSize = get_bucket_size(allocator, bucketIdx);

        SubAllocItem *prevEntry = 0;
        while (entry)
        {
            SubAllocItem *sibling = get_sibling(allocator, entry, bucketSize);
            SubAllocItem *nextEntry = entry->next;
            SubAllocItem *prevSib = 0;
            SubAllocItem *nextSib = 0;
            b32 found = false;

            if (!sibling->isUsed) {
                for (SubAllocItem *test = entry->next;
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
                ++result;
            } else {
                prevEntry = entry;
            }
            entry = nextEntry;
        }
    }

#if SUB_ALLOC_DEBUG
    allocator->coalesceCount += result;
#endif
    return result;
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
        update_mark(allocator, allocator->base + sizeof(SubAllocItem));
#endif

        u32 bucketCount = (highBit.index - MIN_SUBALLOC_BITS + 1);
        i_expect(bucketCount < array_count(allocator->freeLists));
        allocator->bucketCount = bucketCount;

#if 0
        for (u32 bucketIdx = 0; bucketIdx < bucketCount; ++bucketIdx)
        {
            SubAllocItem *list = allocator->freeLists + bucketIdx;
            list->prev = list->next = list;
        }
#endif

        SubAllocItem *baseBucket = (SubAllocItem *)allocator->base;
        add_to_free_list(allocator, baseBucket, 0);

        result = true;
    }

    return result;
}

internal void *
sub_alloc(SubAllocator *allocator, u32 requestSize, MemoryAllocFlags allocInfo)
{
    i_expect(allocator);
    i_expect(allocator->base);

    // NOTE(michiel): We ignore allocInfo.alignment for now, because the suballocator will always be aligned
    // if the requestSize is greater than or equal to the alignment. Which is _probably_ true.

    void *result = 0;
    u32 totalSize = requestSize + SUBALLOC_HEADER_OFFSET;
    b32 clear = !(allocInfo.flags & Memory_NoClear);

    if ((requestSize > 0) && (totalSize <= allocator->totalSize))
    {
        u32 bucket = bucket_from_request(allocator, totalSize);
        u32 origBucket = bucket;
        u32 bucketSize = get_bucket_size(allocator, bucket);

        b32 doSplit = false;
        SubAllocItem *allocPtr = remove_first_free(allocator, bucket);
        while (!allocPtr && bucket) {
            allocPtr = remove_first_free(allocator, --bucket);
            bucketSize <<= 1;
            doSplit = true;
        }

        if (!allocPtr) {
            sub_coalesce(allocator);

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
#if SUB_ALLOC_DEBUG
                    ++allocator->splitCount;
#endif
                    SubAllocItem *sibling = (SubAllocItem *)((u8 *)allocPtr + bucketSize);
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

    i_expect(result < allocator->end);

    if (clear)
    {
        copy_single(requestSize, 0, result);
    }

    return result;
}

internal void *
sub_dealloc(SubAllocator *allocator, void *pointer)
{
    void *result = pointer;

    if (pointer)
    {
        SubAllocItem *entry = (SubAllocItem *)((u8 *)pointer - SUBALLOC_HEADER_OFFSET);
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

internal void *
sub_realloc(SubAllocator *allocator, void *pointer, u32 requestSize, MemoryAllocFlags allocInfo)
{
    void *result = 0;
    b32 clear = !(allocInfo.flags & Memory_NoClear);

    if (pointer)
    {
        SubAllocItem *entry = (SubAllocItem *)((u8 *)pointer - SUBALLOC_HEADER_OFFSET);
        suballoc_expect(entry->isUsed);
        umm entrySize = entry->size - 1 - SUBALLOC_HEADER_OFFSET;

        if (entrySize < requestSize)
        {
            // TODO(michiel): Could check for a free sibling to increase the bucket, then no copy is needed.
            MemoryAllocFlags moddedInfo = allocInfo;
            moddedInfo.flags |= Memory_NoClear;
            void *newPoint = sub_alloc(allocator, requestSize, moddedInfo);
            copy(entrySize, pointer, newPoint);
            if (clear)
            {
                copy_single(requestSize - entrySize, 0, newPoint + entrySize);
            }
            sub_dealloc(allocator, pointer);
            result = newPoint;
        }
        else
        {
            // NOTE(michiel): Nothing to do.
            result = pointer;
        }
    }
    else
    {
        result = sub_alloc(allocator, requestSize);
    }

    return result;
}

internal String
sub_alloc_string(SubAllocator *allocator, u32 size, MemoryAllocFlags allocInfo)
{
    String result = {};
    u8 *mem = (u8 *)sub_alloc(allocator, size + 1, allocInfo);
    if (mem) {
        result.size = size;
        result.data = mem;
    }
    return result;
}

internal String
sub_alloc_string(SubAllocator *allocator, String s)
{
    String result = sub_alloc_string(allocator, s.size, no_clear_memory_alloc());
    copy(s.size, s.data, result.data);
    result.data[result.size] = 0;
    return result;
}

internal String
sub_alloc_string_fmt(SubAllocator *allocator, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    String sizeStr = vstring_fmt(0, 0, fmt, args);
    va_end(args);

    String allocStr = sub_alloc_string(allocator, sizeStr.size, no_clear_memory_alloc());
    va_start(args, fmt);
    String result = string_fmt(allocStr.size + 1, allocStr.data, fmt, args);
    va_end(args);
    i_expect(result.size == sizeStr.size);
    result.data[result.size] = 0;

    return result;
}

internal String
sub_dealloc_string(SubAllocator *allocator, String s)
{
    String result = {};
    result.data = (u8 *)sub_dealloc(allocator, s.data);
    result.size = 0;
    return result;
}
