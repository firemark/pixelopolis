#pragma once
#include <unistd.h>

struct Storage {
    struct Storage* next;
    size_t size;
};

struct MemoryChunk {
    void* chunk;
    struct MemoryChunk* next;
    struct Storage* begin;
    struct Storage* end;
    size_t size;
    size_t allocated_size;
};

struct MemoryChunk* memory_create(size_t chunk_size);
void* memory_allocate(struct MemoryChunk* memory, size_t size);
void* memory_allocate_array(struct MemoryChunk* memory, size_t size, size_t count);
void memory_free(struct MemoryChunk* memory);

#define MEMORY_ALLOCATE(memory, type) memory_allocate(memory, sizeof(type))
#define MEMORY_ALLOCATE_ARRAY(memory, type, c) memory_allocate_array(memory, sizeof(type), c)