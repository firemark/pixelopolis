#pragma once

struct HashStrItem {
    char* key;
    void* value;
};

struct HashMap {
    struct HashStrItem **items;
    int max_size;
    int size;
};

struct HashMap* hash_make();
int hash_set(struct HashMap* map, char* key, void* value, void** removed_value);
void* hash_get(struct HashMap* map, char* key);
void hash_update(struct HashMap* map, struct HashMap* other);
void hash_destroy(struct HashMap* map);

#define HASH_LINE(a) _HASH_CONCAT(a, __LINE__)
#define _HASH_CONCAT(a, b) a ## b

#define _hash_iter(map) \
    int HASH_LINE(hash_iter_i); \
    for (HASH_LINE(hash_iter_i) = 0; \
         HASH_LINE(hash_iter_i) < map->max_size; \
         HASH_LINE(hash_iter_i)++)

#define _hash_item(map) map->items[HASH_LINE(hash_iter_i)]

#define hash_iter(obj, map) \
    _hash_iter(map) if ( \
        (obj = _hash_item(map)) \
    )

#define hash_iter_values(obj, map) \
    struct HashStrItem* HASH_LINE(hash_item); \
    _hash_iter(map) if ( \
        (HASH_LINE(hash_item) = _hash_item(map)), \
        (obj = HASH_LINE(hash_item) ? HASH_LINE(hash_item)->value : NULL) \
    )

#define hash_iter_keys(obj, map) \
    struct HashStrItem* HASH_LINE(hash_item); \
    _hash_iter(map) if ( \
        (HASH_LINE(hash_item) = _hash_item(map)), \
        (obj = HASH_LINE(hash_item) ? HASH_LINE(hash_item)->key : NULL) \
    )
