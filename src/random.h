typedef struct RandomSeriesPCG
{
    u64 state;
    u64 selector;
} RandomSeriesPCG;

typedef struct RandomListEntry
{
    f32 weight;
    u8 *data;
} RandomListEntry;

typedef struct RandomList
{
    RandomSeriesPCG *series;
    f32 totalWeight;

    u32 entryCount;
    RandomListEntry *entries;
} RandomList;

internal RandomSeriesPCG
random_seed_pcg(u64 state, u64 selector)
{
    RandomSeriesPCG result;

    result.state = state;
    result.selector = (selector << 1) | 1;

    return result;
}

internal u32
random_next_u32(RandomSeriesPCG *series)
{
    u64 state = series->state;
    state = state * 6364136223846793005ULL + series->selector; // 0x5851f42d4c957f2d
    series->state = state;

    u32 preRotate = (u32)((state ^ (state >> 18)) >> 27);
    u32 result = rotate_right(preRotate, (s32)(state >> 59));

    return result;
}

internal u32
random_choice(RandomSeriesPCG *series, u32 choiceCount)
{
    u32 result = (random_next_u32(series) % choiceCount);
    return result;
}

internal f32
random_unilateral(RandomSeriesPCG *series)
{
    f32 result = (f32)random_next_u32(series) / (f32)U32_MAX;
    return result;
}

internal f32
random_bilateral(RandomSeriesPCG *series)
{
    f32 result = random_unilateral(series) * 2.0f - 1.0f;
    return result;
}

internal f64
random_unilateral64(RandomSeriesPCG *series)
{
    f64 result = (f64)random_next_u32(series) / (f64)U32_MAX;
    return result;
}

internal f64
random_bilateral64(RandomSeriesPCG *series)
{
    f64 result = random_unilateral64(series) * 2.0 - 1.0;
    return result;
}

#ifdef __cplusplus
internal u32
slow_gaussian_choice(RandomSeriesPCG *series, u32 choiceCount, u32 gaussionCount = 8)
{
    u64 sum = 0;
    for (u32 i = 0; i < gaussionCount; ++i)
    {
        sum += (random_next_u32(series) % choiceCount);
    }
    sum /= gaussionCount;
    return sum;
}
#else
internal u32
slow_gaussian_choice(RandomSeriesPCG *series, u32 choiceCount, u32 gaussionCount)
{
    u64 sum = 0;
    for (u32 i = 0; i < gaussionCount; ++i)
    {
        sum += (random_next_u32(series) % choiceCount);
    }
    sum /= gaussionCount;
    return sum;
}
#endif

internal f32
slow_gaussian(RandomSeriesPCG *series)
{
    f32 f1 = random_bilateral(series);
    f32 f2 = random_bilateral(series);

    f32 s = f1 * f1 + f2 * f2;
    while ((s >= 1.0f) || (s == 0.0f))
    {
        f1 = random_bilateral(series);
        f2 = random_bilateral(series);
        s = f1 * f1 + f2 * f2;
    }
    s = square_root((-2.0f * log(s)) / s);
    return f1 * s;
}

#if 0
internal f32
random_gaussian(RandomSeriesPCG *series, f32 mean = 0.0f, f32 stdDeviation = 1.0f)
{
    f32 f1 = random_bilateral(series);
    f32 f2 = random_bilateral(series);

    f32 s = f1 * f1 + f2 * f2;
    while ((s >= 1.0f) || (s == 0.0f))
    {
        f1 = random_bilateral(series);
        f2 = random_bilateral(series);
        s = f1 * f1 + f2 * f2;
    }
    s = square_root((-2.0f * log(s)) / s);
    return (f1 * s) - mean;
}
#endif

//
// NOTE(michiel): Random lists, based on weights
//

internal RandomList
allocate_rand_list(MemoryAllocator *allocator, u32 count)
{
    RandomList result = {0};
    result.entryCount = count;
    result.entries = allocate_array(allocator, RandomListEntry, count, default_memory_alloc());

    return result;
}

#define init_rand_list(s, l, w, d) init_rand_list_(s, l, w, sizeof(*d), d)
internal void
init_rand_list_(RandomSeriesPCG *series, RandomList *list,
                f32 *weights, u32 dataItemSize, void *data)
{
    list->series = series;
    list->totalWeight = 0.0f;
    u8 *dataAt = (u8 *)data;
    for (u32 listIdx = 0; listIdx < list->entryCount; ++listIdx)
    {
        f32 weight = weights[listIdx];
        list->totalWeight += weight;

        RandomListEntry *entry = list->entries + listIdx;
        entry->weight = weight;
        entry->data = dataAt;
        dataAt += dataItemSize;
    }
}

internal RandomListEntry
random_entry(RandomList *list)
{
    RandomListEntry result = {0};

    f32 val = random_unilateral(list->series) * list->totalWeight;

    f32 curWeight = 0.0f;
    for (u32 entryIdx = 0; entryIdx < list->entryCount; ++entryIdx)
    {
        RandomListEntry *entry = list->entries + entryIdx;
        curWeight += entry->weight;
        if (val < curWeight)
        {
            result.weight = entry->weight;
            result.data = entry->data;
            break;
        }
    }

    return result;
}
