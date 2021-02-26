#define MIN_SUBALLOC_SIZE  (1 << MIN_SUBALLOC_BITS)
compile_expect(MIN_SUBALLOC_SIZE >= sizeof(SubAllocItem));

#define SUBALLOC_HEADER_OFFSET sizeof(umm)

#if SUB_ALLOC_DEBUG
// NOTE(michiel): Use this expect to test every single part of the subAllocator
#define suballoc_expect(...) i_expect(__VA_ARGS__)
#else
#define suballoc_expect(...)
#endif

internal u32
bucket_from_size(SubAllocator *subAllocator, u32 size)
{
    suballoc_expect(is_pow2(size));

    BitScanResult highBit = find_most_significant_set_bit(size);
    suballoc_expect(highBit.found);

    u32 bucket = subAllocator->bucketCount - 1 + MIN_SUBALLOC_BITS - highBit.index;
    return bucket;
}

internal u32
bucket_from_request(SubAllocator *subAllocator, u32 request)
{
    BitScanResult highBit = find_most_significant_set_bit(request);
    suballoc_expect(highBit.found);

    u32 size = (1 << highBit.index);
    if (request > size) {
        size <<= 1;
    }

    u32 bucket = bucket_from_size(subAllocator, size);
    return bucket;
}

internal u32
get_bucket_size(SubAllocator *subAllocator, u32 bucket)
{
    u32 result = 1 << (subAllocator->bucketCount - 1 + MIN_SUBALLOC_BITS - bucket);
    suballoc_expect(bucket_from_size(subAllocator, result) == bucket);
    return result;
}

#if SUB_ALLOC_DEBUG
internal b32
update_mark(SubAllocator *subAllocator, u8 *newValue)
{
    b32 result = true;
    if (newValue > subAllocator->mark)
    {
        if (newValue <= subAllocator->end)
        {
            subAllocator->mark = newValue;
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
add_to_free_list(SubAllocator *subAllocator, SubAllocItem *entry, u32 bucket)
{
#if 0
    // TODO(michiel): Maybe sort the free list? Always return lowest memory first
    SubAllocItem *prev = 0;
    SubAllocItem *item = subAllocator->freeLists[bucket];
    while (item && (item < entry)) {
        prev = item;
        item = item->next;
    }
    entry->next = item;
    if (prev) {
        prev->next = entry;
    } else {
        subAllocator->freeLists[bucket] = entry;
    }
#else
    entry->next = subAllocator->freeLists[bucket];
    subAllocator->freeLists[bucket] = entry;
#endif
}

internal SubAllocItem *
remove_first_free(SubAllocator *subAllocator, u32 bucket)
{
    SubAllocItem *result = subAllocator->freeLists[bucket];

    if (result) {
        subAllocator->freeLists[bucket] = result->next;
        result->next = 0;
    }

    return result;
}

//#if SUB_ALLOC_DEBUG
internal b32
is_right(SubAllocator *subAllocator, SubAllocItem *entry, u32 size)
{
    suballoc_expect(is_pow2(size));
    umm offset = ((u8 *)entry - subAllocator->base);
    return offset & size;
}
//#endif

internal SubAllocItem *
get_sibling(SubAllocator *subAllocator, SubAllocItem *entry, u32 size)
{
    suballoc_expect(is_pow2(size));
    SubAllocItem *result = (SubAllocItem *)(subAllocator->base + (((u8 *)entry - subAllocator->base) ^ size));
    return result;
}

internal u32
sub_coalesce(SubAllocator *subAllocator)
{
    // NOTE(michiel): This returns the number of blocks that got coalesced.
    u32 result = 0;
    for (u32 bucketIdx = subAllocator->bucketCount - 1; bucketIdx > 0; --bucketIdx)
    {
        SubAllocItem *entry = subAllocator->freeLists[bucketIdx];
        u32 bucketSize = get_bucket_size(subAllocator, bucketIdx);

        SubAllocItem *prevEntry = 0;
        while (entry)
        {
            SubAllocItem *sibling = get_sibling(subAllocator, entry, bucketSize);
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
                    subAllocator->freeLists[bucketIdx] = nextEntry;
                }

                entry->next = 0;
                add_to_free_list(subAllocator, (entry < sibling) ? entry : sibling, bucketIdx - 1);
                ++result;
            } else {
                prevEntry = entry;
            }
            entry = nextEntry;
        }
    }

#if SUB_ALLOC_DEBUG
    subAllocator->coalesceCount += result;
#endif
    return result;
}

internal b32
init_sub_allocator(SubAllocator *subAllocator, u32 size, u8 *data)
{
    i_expect((size > (MIN_SUBALLOC_SIZE * 16)) && "Use more than 16x the minimum allocation when using this");
    i_expect((size <= S32_MAX) && "We only support up to 2GB as memory arena");
    i_expect(is_pow2(size) && "Only powers of two are allowed as size");
    i_expect((((umm)data & ~0xF) == (umm)data) && "The data must be 16 bytes aligned");

    b32 result = false;

    subAllocator->totalSize = 0;

    BitScanResult highBit = find_most_significant_set_bit(size);
    if (highBit.found)
    {
        subAllocator->totalSize = size;

        subAllocator->base = data;
        subAllocator->end = subAllocator->base + size;

#if SUB_ALLOC_DEBUG
        subAllocator->mark = subAllocator->base;
        update_mark(subAllocator, subAllocator->base + sizeof(SubAllocItem));
#endif

        u32 bucketCount = (highBit.index - MIN_SUBALLOC_BITS + 1);
        i_expect(bucketCount < array_count(subAllocator->freeLists));
        subAllocator->bucketCount = bucketCount;

#if 0
        for (u32 bucketIdx = 0; bucketIdx < bucketCount; ++bucketIdx)
        {
            SubAllocItem *list = subAllocator->freeLists + bucketIdx;
            list->prev = list->next = list;
        }
#endif

        SubAllocItem *baseBucket = (SubAllocItem *)subAllocator->base;
        add_to_free_list(subAllocator, baseBucket, 0);

        result = true;
    }

    return result;
}

internal ALLOCATE_MEMORY_SIZE(sub_alloc)
{
    SubAllocator *subAllocator = (SubAllocator *)allocator;
    i_expect(subAllocator);
    i_expect(subAllocator->base);

    // NOTE(michiel): We ignore allocInfo.alignment for now, because the suballocator will always be aligned
    // if the size is greater than or equal to the alignment. Which is _probably_ true.

    void *result = 0;
    u32 totalSize = safe_truncate_to_u32(size) + SUBALLOC_HEADER_OFFSET;
    b32 clear = !(flags & Memory_NoClear);

    if ((size > 0) && (totalSize <= subAllocator->totalSize))
    {
        u32 bucket = bucket_from_request(subAllocator, totalSize);
        u32 origBucket = bucket;
        u32 bucketSize = get_bucket_size(subAllocator, bucket);

        b32 doSplit = false;
        SubAllocItem *allocPtr = remove_first_free(subAllocator, bucket);
        while (!allocPtr && bucket) {
            allocPtr = remove_first_free(subAllocator, --bucket);
            bucketSize <<= 1;
            doSplit = true;
        }

        if (!allocPtr) {
            sub_coalesce(subAllocator);

            bucket = origBucket;
            bucketSize = get_bucket_size(subAllocator, bucket);

            doSplit = false;
            allocPtr = remove_first_free(subAllocator, bucket);
            while (!allocPtr && bucket) {
                allocPtr = remove_first_free(subAllocator, --bucket);
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
                    ++subAllocator->splitCount;
#endif
                    SubAllocItem *sibling = (SubAllocItem *)((u8 *)allocPtr + bucketSize);
                    suballoc_expect(is_right(subAllocator, sibling, bucketSize));
                    add_to_free_list(subAllocator, sibling, bucket);

                    ++bucket;
                    bucketSize >>= 1;
                }
                suballoc_expect(get_bucket_size(subAllocator, origBucket) == (bucketSize << 1));
            }
            u32 newSize = get_bucket_size(subAllocator, origBucket);
            allocPtr->size = newSize;
            suballoc_expect((((u8 *)allocPtr - subAllocator->base) & (newSize - 1)) == 0);
            allocPtr->isUsed = true;

#if SUB_ALLOC_DEBUG
            update_mark(subAllocator, (u8 *)allocPtr + newSize);
#endif

            result = (u8 *)allocPtr + SUBALLOC_HEADER_OFFSET;
        }
    }

    i_expect(result < subAllocator->end);

    if (clear)
    {
        copy_single(size, 0, result);
    }

    return result;
}

internal DEALLOCATE_MEMORY(sub_dealloc)
{
    SubAllocator *subAllocator = (SubAllocator *)allocator;
    void *result = memory;

    if (memory)
    {
        SubAllocItem *entry = (SubAllocItem *)((u8 *)memory - SUBALLOC_HEADER_OFFSET);
        suballoc_expect(entry->isUsed);
#if SUB_ALLOC_DEBUG
        suballoc_expect((u8 *)entry < subAllocator->mark);
#endif
        entry->isUsed = false;
        suballoc_expect((entry->size & 0x7) == 0);
        suballoc_expect(entry->size <= S32_MAX);
        suballoc_expect(is_pow2(entry->size));

        u32 bucket = bucket_from_size(subAllocator, (u32)entry->size);
        add_to_free_list(subAllocator, entry, bucket);

        result = 0;
    }

    return result;
}

internal REALLOCATE_MEMORY_SIZE(sub_realloc)
{
    SubAllocator *subAllocator = (SubAllocator *)allocator;

    void *result = 0;
    b32 clear = !(flags & Memory_NoClear);

    if (memory)
    {
        SubAllocItem *entry = (SubAllocItem *)((u8 *)memory - SUBALLOC_HEADER_OFFSET);
        suballoc_expect(entry->isUsed);
        umm entrySize = entry->size - 1 - SUBALLOC_HEADER_OFFSET;

        if (entrySize < size)
        {
            // TODO(michiel): Could check for a free sibling to increase the bucket, then no copy is needed.
            flags |= Memory_NoClear;
            void *newPoint = sub_alloc(subAllocator, size, flags);
            copy(entrySize, memory, newPoint);
            if (clear)
            {
                copy_single(size - entrySize, 0, (u8 *)newPoint + entrySize);
            }
            sub_dealloc(subAllocator, memory);
            result = newPoint;
        }
        else
        {
            // NOTE(michiel): Nothing to do.
            result = memory;
        }
    }
    else
    {
        result = sub_alloc(subAllocator, size, flags);
    }

    return result;
}

internal String
sub_alloc_string(SubAllocator *subAllocator, u32 size, u32 flags)
{
    String result = {};
    u8 *mem = (u8 *)sub_alloc(subAllocator, size + 1, flags);
    if (mem) {
        result.size = size;
        result.data = mem;
    }
    return result;
}

internal ALLOCATE_MEMORY_STRINGZ(sub_alloc_stringz)
{
    SubAllocator *subAllocator = (SubAllocator *)allocator;
    String result = sub_alloc_string(subAllocator, safe_truncate_to_u32(source.size), align_memory_alloc(1, false));
    copy(source.size, source.data, result.data);
    result.data[result.size] = 0;
    return result;
}

internal String
sub_alloc_string_fmt(SubAllocator *subAllocator, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    String sizeStr = string_vformatter(0, 0, fmt, args);
    va_end(args);

    String allocStr = sub_alloc_string(subAllocator, safe_truncate_to_u32(sizeStr.size), no_clear_memory_alloc());
    va_start(args, fmt);
    String result = string_vformatter(allocStr.size + 1, (char *)allocStr.data, fmt, args);
    va_end(args);
    i_expect(result.size == sizeStr.size);
    result.data[result.size] = 0;

    return result;
}

internal String
sub_dealloc_string(SubAllocator *subAllocator, String s)
{
    String result = {};
    result.data = (u8 *)sub_dealloc(subAllocator, s.data);
    result.size = 0;
    return result;
}
