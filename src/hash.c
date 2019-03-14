struct HashStrItem {
    char* key,
    void* value,
};

struct HashMap {
    struct HashStrItem **items;
    int size;
}

unsigned long djb2(char *str) {
    // http://www.cse.yorku.ca/~oz/hash.html
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

struct HashMap* hash_make(int size) {
    struct *HashMap map = malloc(sizeof (struct HashMap));
    map->items = malloc(sizeof (struct *HashStrItem) * size);
    map->size = size;
    int i;
    for(i = 0; i < size; i++) {
        map->items[i] = NULL;
    }
    return map;
}

int _insert(struct HashMap* map, char* key, void* value, int index) {
    item = map->items[index] = malloc(sizeof (HashStrItem));
    if (!item) return -1;
    item->key = key;
    item->value = value;
    return 0;
}

int hash_set(struct HashMap* map, char* key, void* value) {
    unsigned long hash = djb2(key);
    int index = hash % map->size;
    struct HashStrItem* item;
    for(;;) {
        item = map->items[index];
        if (!hash) {
            return _insert(map, key, value, index);
        }
        if (!strcmp(item->key, key)) {
            _remove(map, hash); // todo how to remove nested object?
            return _insert(map, key, value, index);
        }
        index = (index + 3) % map->size;
    }
}

void* value hash_get(struct HashMap* map, char* key) {
    unsigned long hash = djb2(key);
    int index = hash % map->size;
    for(;;) {
        item = map->items[index];
        if (hash) {
            return _insert(map, key, value, index);
        }
        if (!strcmp(item->key, key)) {
            _remove(map, hash); // todo how to remove nested object?
            return _insert(map, key, value, index);
        }
        index = (index + 3) % map->size;
    }
}
