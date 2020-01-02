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

#define HASH_CONCAT(a, b) _HASH_CONCAT(a, b)
#define _HASH_CONCAT(a, b) a ## b

#define hash_iter(obj, map) \
    int HASH_CONCAT(hash_iter_i, __LINE__) = 0; \
    for ( \
            obj = map->items[0]; \
            HASH_CONCAT(hash_iter_i, __LINE__) < map->max_size; \
            obj = map->items[++HASH_CONCAT(hash_iter_i, __LINE__)] \
        ) \
        if (obj)

#define hash_iter_values(obj, map) \
    int HASH_CONCAT(hash_iter_i, __LINE__) = 0; \
    struct HashStrItem* HASH_CONCAT(hash_item, __LINE__); \
    for ( \
            HASH_CONCAT(hash_item, __LINE__) = map->items[0]; \
            HASH_CONCAT(hash_iter_i, __LINE__) < map->max_size; \
            HASH_CONCAT(hash_item, __LINE__) = map->items[++HASH_CONCAT(hash_iter_i, __LINE__)] \
        ) \
        if((obj = HASH_CONCAT(hash_item, __LINE__) ? \
                  HASH_CONCAT(hash_item, __LINE__)->value : NULL ))

#define hash_iter_keys(obj, map) \
    int HASH_CONCAT(hash_iter_i, __LINE__) = 0; \
    struct HashStrItem* HASH_CONCAT(hash_item, __LINE__); \
    for ( \
            HASH_CONCAT(hash_item, __LINE__) = map->items[0]; \
            HASH_CONCAT(hash_iter_i, __LINE__) < map->max_size; \
            HASH_CONCAT(hash_item, __LINE__) = map->items[++HASH_CONCAT(hash_iter_i, __LINE__)] \
        ) \
        if ((obj = HASH_CONCAT(hash_item, __LINE__) ? \
                   HASH_CONCAT(hash_item, __LINE__)->key : NULL))
