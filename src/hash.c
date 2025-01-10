#include "pixelopolis/hash.h"

#include <stdlib.h>
#include <string.h>

unsigned long djb2(char* str) {
    // http://www.cse.yorku.ca/~oz/hash.html
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

static void _fill_null(struct HashStrItem** items, int new_size) {
    int i;
    for (i = 0; i < new_size; i++) {
        items[i] = NULL;
    }
}

struct HashMap* hash_make_with_memory(struct Memory* memory) {
    struct HashMap* map;
    int max_size = 32;
    if (memory) {
        map = MEMORY_ALLOCATE(memory, struct HashMap);
        map->items = MEMORY_ALLOCATE_ARRAY(memory, struct HashStrItem*, max_size);
    } else {
        map = malloc(sizeof(struct HashMap));
        map->items = malloc(sizeof(struct HashStrItem*) * max_size);
    }

    map->size = 0;
    map->memory = memory;
    map->max_size = max_size;
    _fill_null(map->items, max_size);
    return map;
}

struct HashMap* hash_make(void) { return hash_make_with_memory(NULL); }

// static void* _remove(struct HashMap* map, int index) {
//     struct HashStrItem* item = map->items[index];
//     void* value = item->value;
//     map->items[index] = NULL;
//     if (!map->memory) {
//         free(item);
//     }
//     return value;
// }

static int _insert(struct HashMap* map, char* key, void* value, int index) {
    struct HashStrItem* item;
    if (map->memory) {
        item = MEMORY_ALLOCATE(map->memory, struct HashStrItem);
    } else {
        item = malloc(sizeof(struct HashStrItem));
    }
    if (!item) return -1;

    map->items[index] = item;
    item->key = key;
    item->value = value;
    return 0;
}

static unsigned long _probing(unsigned long hash, unsigned long shift) {
    return hash + shift * shift + 3L * shift;
}
#include <stdio.h>

static struct HashStrItem** _hash_cpy_items(size_t new_size, struct HashMap* map) {
    // make new table because index has changed
    struct HashStrItem** new_items;
    if (map->memory) {
        new_items = MEMORY_ALLOCATE_ARRAY(map->memory, struct HashStrItem*, new_size);
    } else {
        new_items = malloc(sizeof(struct HashStrItem*) * new_size);
    }
    _fill_null(new_items, new_size);

    unsigned long hash;
    unsigned long shift;
    struct HashStrItem* item;
    hash_iter (item, map) {
        hash = djb2(item->key);
        shift = 0;
        for (;;) {
            int index = _probing(hash, shift++) % new_size;
            if (!new_items[index]) {
                new_items[index] = item;
                break;
            }
        }
    }

    return new_items;
}

static int _resize_if_need(struct HashMap* map) {
    float load = (float)(map->size) / (float)(map->max_size);
    if (load < 0.8) return 0;
    int old_size = map->max_size;
    int new_size = old_size * 2;

    struct HashStrItem** new_items = _hash_cpy_items(new_size, map);

    if (!map->memory) {
        free(map->items);
    }
    map->items = new_items;
    map->max_size = new_size;
    return -1;
}

static inline int if_key_eq(struct HashStrItem* item, char* key) { return strcmp(item->key, key) == 0; }

int hash_set(struct HashMap* map, char* key, void* value, void** removed_value) {
    unsigned long hash = djb2(key);
    unsigned long shift;
start:
    shift = 0;

    for (;;) {
        int index = _probing(hash, shift) % map->max_size;
        struct HashStrItem*  item = map->items[index];
        if (!item) {
            if (_resize_if_need(map)) {
                goto start;
            }
            map->size++;
            if (removed_value) *removed_value = NULL;
            char* new_key; 
            {
                // Make copy of key.
                size_t key_count = strlen(key) + 1;
                if (map->memory) {
                    new_key = MEMORY_ALLOCATE_ARRAY(map->memory, char, key_count);
                } else {
                    new_key = malloc(sizeof(char) * key_count);
                }
                strncpy(new_key, key, key_count);
            }
            return _insert(map, new_key, value, index);
        }
        if (if_key_eq(item, key)) {
            if (removed_value) *removed_value = item->value;
            item->value = value;
            return 0;
        }
        shift++;
    }
}

void* hash_get(struct HashMap* map, char* key) {
    unsigned long hash = djb2(key);
    unsigned long shift = 0;
    struct HashStrItem* item;

    int index;
    for (;;) {
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
    if (map->memory) {
        return;
    }
    int i;
    for (i = 0; i < map->max_size; i++) {
        struct HashStrItem *item = map->items[i];
        if (item) {
            free(item->key);
            free(item);
        }
    }
    free(map->items);
    free(map);
}

void hash_update(struct HashMap* map, struct HashMap* other) {
    struct HashStrItem* item;
    hash_iter (item, other) {
        hash_set(map, item->key, item->value, NULL);
    }
}
