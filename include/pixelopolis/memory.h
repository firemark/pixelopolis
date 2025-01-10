#pragma once
#include <stdint.h>
#include <unistd.h>

struct MemoryChunk {
    uint8_t* chunk;
    uint8_t* ptr;
    struct MemoryChunk* next;
    size_t allocated_size;
};

struct Memory {
    size_t chunk_size;
    struct MemoryChunk* begin;
    struct MemoryChunk* end;
};

struct Memory* memory_create(size_t chunk_size);
void* memory_allocate(struct Memory* memory, size_t size);
void* memory_allocate_array(struct Memory* memory, size_t size, size_t count);
size_t memory_size(struct Memory* memory);
void memory_free(struct Memory* memory);

#define MEMORY_ALLOCATE(memory, type) (type*)memory_allocate(memory, sizeof(type))
#define MEMORY_ALLOCATE_ARRAY(memory, type, c) (type*)memory_allocate_array(memory, sizeof(type), c)