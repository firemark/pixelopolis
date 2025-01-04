#include "pixelopolis/memory.h"

#include <assert.h>
#include <stdlib.h>

#define ALIGN_SIZE 8
#define ALIGN(size) (((size) + (ALIGN_SIZE - 1)) & ~(ALIGN_SIZE - 1))

static struct MemoryChunk* _memory_chunk_create(size_t chunk_size) {
    struct MemoryChunk* memory = malloc(sizeof(struct MemoryChunk));
    size_t aligned_size = ALIGN(chunk_size);
    memory->chunk = malloc(aligned_size);
    memory->allocated_size = 0;
    memory->begin = NULL;
    memory->end = NULL;
    return memory;
}

struct Memory* memory_create(size_t chunk_size) {
    chunk_size = ALIGN(chunk_size);
    struct Memory* memory = malloc(sizeof(struct Memory));
    struct MemoryChunk* chunk = _memory_chunk_create(chunk_size);
    memory->chunk_size = chunk_size;
    memory->begin = chunk;
    memory->end = chunk;
    return memory;
}

void* memory_allocate(struct Memory* memory, size_t size) {
    struct Storage* storage;
    struct MemoryChunk* chunk = memory->end;
    size_t size_aligned = ALIGN(size);

    if (!chunk->begin) {
        assert(size < memory->chunk_size);

        storage = malloc(sizeof(struct Storage));
        storage->next = NULL;
        storage->size = size_aligned;
        storage->ptr = chunk->chunk;

        chunk->begin = storage;
        chunk->end = storage;
        chunk->allocated_size = size_aligned;
    } else {
        size_t new_allocated_size = chunk->allocated_size + size_aligned;
        if (new_allocated_size > memory->chunk_size) {
            // Create new memory chunk.
            struct MemoryChunk* new_chunk = _memory_chunk_create(memory->chunk_size);
            chunk->next = new_chunk;
            memory->end = new_chunk;
            memory_allocate(memory, size);
        }

        storage = malloc(sizeof(struct Storage));
        storage->next = NULL;
        storage->size = size_aligned;
        storage->ptr = chunk->end->ptr + chunk->end->size;

        chunk->end->next = storage;
        chunk->end = storage;
        chunk->allocated_size = new_allocated_size;
    }
    return storage->ptr;
}

void* memory_allocate_array(struct Memory* memory, size_t size, size_t count) {
    return memory_allocate(memory, size * count);
}

void memory_free(struct Memory* memory) {
    struct MemoryChunk* chunk = memory->begin;
    while (chunk) {
        struct Storage* storage = chunk->begin;
        while (storage) {
            struct Storage* temp = storage;
            storage = storage->next;
            free(temp);
        }
        struct MemoryChunk* temp = chunk;
        chunk = chunk->next;
        free(temp);
    }
    free(memory);
}