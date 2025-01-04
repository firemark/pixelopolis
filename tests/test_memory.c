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


Suite* memory_suite(void) {
    Suite* suite = suite_create("memory");
    TCase* tcase = tcase_create("memory");

    tcase_add_test(tcase, test_memory_create);
    suite_add_tcase(suite, tcase);
    return suite;
}