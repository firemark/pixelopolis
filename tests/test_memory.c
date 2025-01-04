#include "suites.h"

#include "pixelopolis/memory.h"

START_TEST(test_memory_create) {
    struct Memory* memory = memory_create(1024);

    ck_assert_int_eq(memory->chunk_size, 1024);
    ck_assert_ptr_ne(memory->begin, NULL);
    ck_assert_ptr_ne(memory->end, NULL);
    ck_assert_ptr_eq(memory->begin, memory->end);

    struct MemoryChunk* chunk = memory->begin;
    ck_assert_ptr_eq(chunk->begin, NULL);
    ck_assert_ptr_eq(chunk->end, NULL);
    ck_assert_int_eq(chunk->allocated_size, 0);

    memory_free(memory);
}
END_TEST

struct Dummy { char a[512]; };

START_TEST(test_memory_allocate_once) {
    struct Memory* memory = memory_create(1024);
    void* ptr = MEMORY_ALLOCATE(memory, struct Dummy);

    ck_assert_ptr_ne(memory->begin, NULL);
    ck_assert_ptr_ne(memory->end, NULL);
    ck_assert_ptr_eq(memory->begin, memory->end);

    struct MemoryChunk* chunk = memory->begin;
    ck_assert_ptr_ne(chunk->begin, NULL);
    ck_assert_ptr_ne(chunk->end, NULL);
    ck_assert_ptr_eq(chunk->begin, chunk->end);
    ck_assert_int_eq(chunk->allocated_size, sizeof(struct Dummy));

    struct Storage* storage = chunk->begin;
    ck_assert_ptr_eq(storage->next, NULL);
    ck_assert_int_eq(storage->size, sizeof(struct Dummy));

    ck_assert_ptr_eq(ptr, storage->ptr);
    ck_assert_ptr_eq(ptr, chunk->chunk);

    memory_free(memory);
}
END_TEST

START_TEST(test_memory_allocate_twice) {
    struct Memory* memory = memory_create(1024);
    void* ptr_a = MEMORY_ALLOCATE(memory, struct Dummy);
    void* ptr_b = MEMORY_ALLOCATE(memory, struct Dummy);

    struct MemoryChunk* chunk = memory->begin;
    ck_assert_int_eq(chunk->allocated_size, sizeof(struct Dummy) * 2);

    struct Storage* first_storage = chunk->begin;
    ck_assert_ptr_ne(first_storage, NULL);
    ck_assert_ptr_ne(first_storage->next, NULL);

    struct Storage* second_storage = first_storage->next;
    ck_assert_ptr_eq(second_storage->next, NULL);

    ck_assert_ptr_eq(ptr_a, first_storage->ptr);
    ck_assert_ptr_eq(ptr_a, chunk->chunk);

    ck_assert_ptr_eq(ptr_b, second_storage->ptr);
    ck_assert_ptr_eq(ptr_b, chunk->chunk + sizeof(struct Dummy));

    memory_free(memory);
}
END_TEST

START_TEST(test_memory_allocate_with_new_chunk) {
    struct Memory* memory = memory_create(1024);
    void* ptr_a = MEMORY_ALLOCATE(memory, struct Dummy);
    void* ptr_b = MEMORY_ALLOCATE(memory, struct Dummy);
    void* ptr_c = MEMORY_ALLOCATE(memory, struct Dummy);

    ck_assert_ptr_eq(memory->begin->next, memory->end);

    struct MemoryChunk* first_chunk = memory->begin;
    ck_assert_ptr_ne(first_chunk->begin, NULL);
    ck_assert_int_eq(first_chunk->allocated_size, sizeof(struct Dummy) * 2);

    struct MemoryChunk* second_chunk = memory->end;
    ck_assert_ptr_ne(second_chunk->begin, NULL);
    ck_assert_int_eq(second_chunk->allocated_size, sizeof(struct Dummy));

    struct Storage* first_storage = first_chunk->begin;
    ck_assert_ptr_ne(first_storage->next, NULL);

    struct Storage* second_storage = first_storage->next;
    ck_assert_ptr_eq(second_storage->next, NULL);

    struct Storage* third_storage = second_chunk->begin;
    ck_assert_ptr_eq(third_storage->next, NULL);

    ck_assert_ptr_eq(ptr_a, first_storage->ptr);
    ck_assert_ptr_eq(ptr_a, first_chunk->chunk);

    ck_assert_ptr_eq(ptr_b, second_storage->ptr);
    ck_assert_ptr_eq(ptr_b, first_chunk->chunk + sizeof(struct Dummy));

    ck_assert_ptr_eq(ptr_c, third_storage->ptr);
    ck_assert_ptr_eq(ptr_c, second_chunk->chunk);

    memory_free(memory);
}
END_TEST


Suite* memory_suite(void) {
    Suite* suite = suite_create("memory");
    TCase* tcase = tcase_create("memory");

    tcase_add_test(tcase, test_memory_create);
    tcase_add_test(tcase, test_memory_allocate_once);
    tcase_add_test(tcase, test_memory_allocate_twice);
    tcase_add_test(tcase, test_memory_allocate_with_new_chunk);
    suite_add_tcase(suite, tcase);
    return suite;
}