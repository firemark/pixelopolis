#include "hash.h"

#include <string.h>
#include <stdlib.h>

unsigned long djb2(char *str) {
    // http://www.cse.yorku.ca/~oz/hash.html
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void _fill_null(struct HashMap* map, int old_size, int new_size) {
    map->max_size = new_size;
    int i;
    for(i = old_size; i < new_size; i++) {
        map->items[i] = NULL;
    }
}

struct HashMap* hash_make(void) {
    struct HashMap* map = malloc(sizeof (struct HashMap));
    int size = 32;
    map->items = malloc(sizeof (struct HashStrItem*) * size);
    map->size = 0;
    _fill_null(map, 0, size);
    return map;
}

void* _remove(struct HashMap* map, int index) {
    struct HashStrItem* item = map->items[index];
    void* value = item->value;
    map->items[index] = NULL;
    free(item);
    return value;
} 

int _insert(struct HashMap* map, char* key, void* value, int index) {
    struct HashStrItem *item = malloc(sizeof (struct HashStrItem)); 
    if (!item) return -1;
    map->items[index] = item;
    item->key = key;
    item->value = value;
    return 0;
}

unsigned long _probing(unsigned long hash, unsigned long shift) {
    return hash + shift * shift + 3L * shift;
}

void _resize_if_need(struct HashMap* map) {
    float load = (float)(map->size) / (float)(map->max_size);
    if (load < 0.8) return;
    int old_size = map->max_size;
    int new_size = old_size * 2;
    int i;

    map->items = realloc(map->items, sizeof (struct HashStrItem*) * new_size);
    _fill_null(map, old_size, new_size);
}

int if_key_eq(struct HashStrItem* item, char* key) {
    return !strcmp(item->key, key);
}

int hash_set(struct HashMap* map, char* key, void* value, void** removed_value) {
    unsigned long hash = djb2(key);
    unsigned long shift = 0;
    struct HashStrItem* item;

    int index;
    for(;;) {
        index = _probing(hash, shift) % map->max_size;
        item = map->items[index];
        if (!item) {
            _resize_if_need(map);
            map->size++;
            if (removed_value) *removed_value = NULL;
            return _insert(map, key, value, index);
        }
        if (if_key_eq(item, key)) {
            if (removed_value) *removed_value = _remove(map, index);
            return _insert(map, key, value, index);
        }
        shift++;
    }
}

void* hash_get(struct HashMap* map, char* key) {
    unsigned long hash = djb2(key);
    unsigned long shift = 0;
    struct HashStrItem *item;

    int index;
    for(;;) {
        index = _probing(hash, shift) % map->max_size;
        item = map->items[index];
        if (!item) {
            return NULL;
        }
        if (if_key_eq(item, key)) {
            return item->value;
        }
        shift++;
    }
}

void hash_destroy(struct HashMap* map) {
    int i;
    for(i = 0; i < map->max_size; i++) {
        free(map->items[i]);
    }
    free(map->items);
    free(map);
}
