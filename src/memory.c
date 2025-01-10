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
    memory->ptr = memory->chunk;
    memory->next = NULL;
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
    struct MemoryChunk* chunk = memory->end;
    size_t size_aligned = ALIGN(size);

    assert(size < memory->chunk_size);
    size_t new_allocated_size = chunk->allocated_size + size_aligned;
    if (new_allocated_size > memory->chunk_size) {
        // Create new memory chunk.
        struct MemoryChunk* new_chunk = _memory_chunk_create(memory->chunk_size);
        memory->end->next = new_chunk;
        memory->end = new_chunk;
        return memory_allocate(memory, size);
    }

    void* ptr = chunk->ptr;
    chunk->allocated_size = new_allocated_size;
    chunk->ptr += size_aligned;
    return ptr;
}

void* memory_allocate_array(struct Memory* memory, size_t size, size_t count) {
    return memory_allocate(memory, size * count);
}

size_t memory_size(struct Memory* memory) {
    size_t size = 0;
    struct MemoryChunk* chunk;
    for(chunk = memory->begin; chunk != NULL; chunk = chunk->next) {
        size += chunk->allocated_size;
    }
    return size;
}

void memory_free(struct Memory* memory) {
    struct MemoryChunk* chunk = memory->begin;
    while (chunk) {
        struct MemoryChunk* temp = chunk;
        free(temp->chunk);
        chunk = chunk->next;
        free(temp);
    }
    free(memory);
}