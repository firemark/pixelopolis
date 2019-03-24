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
void hash_destroy(struct HashMap* map);

#define hash_iter(obj, map) \
    int hash_iter_i##__FILE__##__LINE__ = 0; \
    for(; hash_iter_i##__FILE__##__LINE__ < map->max_size; \
            obj = map->items[hash_iter_i##__FILE__##__LINE__++]) \
        if (obj)

#define hash_iter_values(obj, map) \
    int hash_iter_i##__FILE__##__LINE__ = 0; \
    struct HashStrItem* hash_item##__FILE__##__LINE__; \
    for(; hash_iter_i##__FILE__##__LINE__ < map->max_size; \
            hash_item##__FILE__##__LINE__ = map->items[hash_iter_i##__FILE__##__LINE__++]) \
        if (obj = hash_item##__FILE__##__LINE__ ? hash_item##__FILE__##__LINE__->value : NULL)

#define hash_iter_keys(obj, map) \
    int hash_iter_i##__FILE__##__LINE__ = 0; \
    struct HashStrItem* hash_item##__FILE__##__LINE__; \
    for(; hash_iter_i##__FILE__##__LINE__ < map->max_size; \
            hash_item##__FILE__##__LINE__ = map->items[hash_iter_i##__FILE__##__LINE__++]) \
        if (obj = hash_item##__FILE__##__LINE__ ? hash_item##__FILE__##__LINE__->key : NULL)
