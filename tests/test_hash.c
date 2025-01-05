#include "suites.h"

#include "pixelopolis/hash.h"

START_TEST(test_hash_make) {
    struct HashMap* map = hash_make();
    ck_assert_int_eq(map->size, 0);
    ck_assert_int_eq(map->max_size, 32);
    int i;
    for (i = 0; i < 32; i++) {
        ck_assert_ptr_eq(map->items[i], NULL);
    }
    hash_destroy(map);
}
END_TEST

START_TEST(test_hash_set) {
    struct HashMap* map = hash_make();
    int has_err = hash_set(map, "key", "value", NULL);

    ck_assert_int_eq(has_err, 0);
    ck_assert_int_eq(map->size, 1);
    int item_index = 14;  // djb2("key") % 32 == 14
    struct HashStrItem* item = map->items[item_index];
    ck_assert_ptr_ne(item, NULL);
    ck_assert_str_eq(item->key, "key");
    ck_assert_str_eq(item->value, "value");
    hash_destroy(map);
}
END_TEST

START_TEST(test_hash_set_with_same_key) {
    struct HashMap* map = hash_make();
    char* old_value;
    hash_set(map, "key", "old_value", NULL);
    hash_set(map, "key", "new_value", (void*)&old_value);

    ck_assert_int_eq(map->size, 1);
    int item_index = 14;  // djb2("key") % 32 == 14
    struct HashStrItem* item = map->items[item_index];
    ck_assert_ptr_ne(item, NULL);
    ck_assert_str_eq(item->key, "key");
    ck_assert_str_eq(item->value, "new_value");
    ck_assert_str_eq(old_value, "old_value");
    hash_destroy(map);
}
END_TEST

START_TEST(test_hash_set_with_collision) {
    struct HashMap* map = hash_make();
    hash_set(map, "key", "old_value", NULL);
    hash_set(map, "KEY", "new_value", NULL);  // collision!

    ck_assert_int_eq(map->size, 2);
    int old_item_index = 14;  // djb2("key") % 32 == 14
    struct HashStrItem* old_item = map->items[old_item_index];
    ck_assert_ptr_ne(old_item, NULL);
    ck_assert_str_eq(old_item->key, "key");
    ck_assert_str_eq(old_item->value, "old_value");

    int new_item_index = 18;  // djb2("KEY") % 32 + probing == 14 + 4 == 18
    struct HashStrItem* new_item = map->items[new_item_index];
    ck_assert_ptr_ne(new_item, NULL);
    ck_assert_str_eq(new_item->key, "KEY");
    ck_assert_str_eq(new_item->value, "new_value");
    hash_destroy(map);
}
END_TEST

START_TEST(test_hash_set_with_resize) {
    struct HashMap* map = hash_make();
    char i;
    char key[5] = "keyA";
    for (i = 0; i < 27; i++) {
        key[3] = 'A' + i;
        hash_set(map, key, "value", NULL);
    }

    ck_assert_int_eq(map->size, 27);
    ck_assert_int_eq(map->max_size, 64);
    hash_destroy(map);
}
END_TEST

START_TEST(test_hash_get) {
    struct HashMap* map = hash_make();
    hash_set(map, "key", "value", NULL);
    char* value = hash_get(map, "key");

    ck_assert_str_eq(value, "value");
    hash_destroy(map);
}
END_TEST

START_TEST(test_hash_get_not_exists) {
    struct HashMap* map = hash_make();
    char* value = hash_get(map, "key");

    ck_assert_ptr_eq(value, NULL);
    hash_destroy(map);
}
END_TEST

START_TEST(test_hash_get_with_collision) {
    struct HashMap* map = hash_make();
    hash_set(map, "key", "old_value", NULL);
    hash_set(map, "KEY", "new_value", NULL);  // collision!

    char* value = hash_get(map, "KEY");
    ck_assert_str_eq(value, "new_value");
    hash_destroy(map);
}
END_TEST

Suite* hash_suite(void) {
    Suite* suite = suite_create("hash");
    TCase* tcase = tcase_create("hash");

    tcase_add_test(tcase, test_hash_make);
    tcase_add_test(tcase, test_hash_set);
    tcase_add_test(tcase, test_hash_set_with_same_key);
    tcase_add_test(tcase, test_hash_set_with_collision);
    tcase_add_test(tcase, test_hash_set_with_resize);
    tcase_add_test(tcase, test_hash_get);
    tcase_add_test(tcase, test_hash_get_not_exists);
    tcase_add_test(tcase, test_hash_get_with_collision);
    suite_add_tcase(suite, tcase);
    return suite;
}