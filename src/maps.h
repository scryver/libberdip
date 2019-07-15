//
// NOTE(michiel): Map
//

// NOTE(michiel): Hashmap/dict implementation to map some key to a value (both must be non-zero)
typedef struct Map
{
    u64 *keys;
    u64 *values;
    u32 len;
    u32 cap;
} Map;

// NOTE(michiel): map_(get/put)         => key = ptr, val = ptr
// NOTE(michiel): map_(get/put)_u64     => key = ptr, val = u64
// NOTE(michiel): map_u64_(get/put)     => key = u64, val = ptr
// NOTE(michiel): map_u64_(get/put)_u64 => key = u64, val = u64
#define map_get(map, key)            ptr_from_u64(map_u64_get_u64(map, u64_from_ptr(key)))
#define map_put(map, key, val)       map_u64_put_u64(map, u64_from_ptr(key), u64_from_ptr(val))
#define map_remove(map, key)         map_u64_remove(map, u64_from_ptr(key))
#define map_get_u64(map, key)        map_u64_get_u64(map, u64_from_ptr(key))
#define map_put_u64(map, key, val)   map_u64_put_u64(map, u64_from_ptr(key), val)
#define map_remove_u64(map, key)     map_u64_remove(map, u64_from_ptr(key))
#define map_u64_get(map, key)        ptr_from_u64(map_u64_get_u64(map, key))
#define map_u64_put(map, key, val)   map_u64_put_u64(map, key, u64_from_ptr(val))

#define map_u64_remove_u64           map_u64_remove

#define MAP_EMPTY_KEY                0xFFFFFFFFFFFFFFFFULL
#define MAP_DELETED_KEY              0xFFFFFFFFFFFFFFFEULL

internal void
map_free(Map *map)
{
    deallocate(map->keys);
    deallocate(map->values);
}

internal inline u64
map_u64_get_u64(Map *map, u64 key)
{
    u64 result = 0;
    if (map->len > 0) {
        i_expect(is_pow2(map->cap));
        umm hash = (umm)hash_u64(key);
        i_expect(map->len < map->cap);
        for (;;) {
            hash &= map->cap - 1;
            if (map->keys[hash] == key) {
                result = map->values[hash];
                break;
            } else if (map->keys[hash] == MAP_EMPTY_KEY) {
                break;
            }
            ++hash;
        }
    }

    return result;
}

internal inline void map_u64_put_u64(Map *map, u64 key, u64 value);

internal void
map_grow(Map *map, umm newCap)
{
    newCap = maximum(newCap, 16);
    i_expect(is_pow2(newCap));

    Map newMap = {0};
    newMap.keys = allocate_array(u64, newCap, Alloc_NoClear);
    newMap.values = allocate_array(u64, newCap, Alloc_NoClear);
    newMap.cap = safe_truncate_to_u32(newCap);
    // TODO(michiel): Do something with different copy_single functions for different value lengths
    copy_single(newCap * sizeof(u64), 0xFF, newMap.keys);

    // NOTE(michiel): Reissue the insertions into our bigger map
    for (u32 mapIndex = 0; mapIndex < map->cap; ++mapIndex) {
        if (map->keys[mapIndex] < MAP_DELETED_KEY) {
            map_u64_put_u64(&newMap, map->keys[mapIndex], map->values[mapIndex]);
        }
    }
    map_free(map);
    *map = newMap;
}

internal inline void
map_u64_put_u64(Map *map, u64 key, u64 value)
{
    if ((4 * map->len) >= (3 * map->cap)) {
        map_grow(map, 2 * map->cap);
    }

    i_expect((4 * map->len) < (3 * map->cap));
    i_expect(is_pow2(map->cap));

    umm hash = (umm)hash_u64(key);
    for (;;) {
        hash &= map->cap - 1;
        if ((map->keys[hash] == MAP_EMPTY_KEY) ||
            (map->keys[hash] == MAP_DELETED_KEY))
        {
            ++map->len;
            map->keys[hash] = key;
            map->values[hash] = value;
            break;
        } else if (map->keys[hash] == key) {
            map->values[hash] = value;
            break;
        }
        ++hash;
    }
}

internal inline void
map_u64_remove(Map *map, u64 key)
{
    if (map->len > 0) {
        i_expect(is_pow2(map->cap));

        umm hash = (umm)hash_u64(key);
        i_expect(map->len < map->cap);

        for (;;) {
            hash &= map->cap - 1;
            if (map->keys[hash] == key) {
                --map->len;
                map->keys[hash] = MAP_DELETED_KEY;
                break;
            } else if (map->keys[hash] == MAP_EMPTY_KEY) {
                break;
            }
            ++hash;
        }
    }
}
