#include "pixelopolis/memory.h"

#include <stdlib.h>

#define ALIGN_SIZE 8
#define ALIGN(size) (((size) + (ALIGN_SIZE - 1)) & ~(ALIGN_SIZE - 1))

struct MemoryChunk* memory_create(size_t chunk_size) {
    struct MemoryChunk* memory = malloc(sizeof(struct MemoryChunk));
    chunk_size = ALIGN(chunk_size);
    memory->chunk = malloc(chunk_size);
    memory->size = chunk_size;
    memory->allocated_size = 0;
    memory->begin = NULL;
    memory->end = NULL;
    return memory;
}

void* memory_allocate(struct MemoryChunk* memory, size_t size) {
    struct Storage* storage;
    ssize_t size_aligned = ALIGN(size);

    // Find last chunk.
    while (memory->next) memory = memory->next;

    if (!memory->begin) {
        assert(size < memory->size);

        storage = malloc(sizeof(struct Storage));
        storage->next = NULL;
        storage->size = size_aligned;
        storage->ptr = memory->chunk;

        memory->begin = storage;
        memory->end = storage;
        memory->allocated_size = size_aligned;
    } else {
        size_t new_allocated_size = memory->allocated_size + size_aligned;
        if (new_allocated_size > memory->size) {
            // Create new memory chunk.
            memory->next = memory_create(memory->size);
            memory_allocate(memory->next, size);
        }

        storage = malloc(sizeof(struct Storage));
        storage->next = NULL;
        storage->size = size_aligned;
        storage->ptr = memory->end->ptr + memory->end->size;

        memory->end->next = storage;
        memory->end = storage;
        memory->allocated_size = new_allocated_size;
    }
    return storage->ptr;
}

void* memory_allocate_array(struct MemoryChunk* memory, size_t size, size_t count) {
    return memory_allocate(memory, size * count);
}

void memory_free(struct MemoryChunk* memory) {
    while (memory) {
        struct Storage* storage = memory->begin;
        while (storage) {
            struct Storage* temp = storage;
            storage = storage->next;
            free(temp);
        }
        struct Memory* temp = memory;
        memory = memory->next;
        free(temp);
    }
}