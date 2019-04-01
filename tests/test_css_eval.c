#include <stdlib.h>
#include <stdio.h>
#include <check.h>

#include "css_eval.h"
#include "css_func.h"

struct Program* _make_program(char* buffer) {
    FILE *stream = fmemopen(buffer, strlen(buffer), "r");
    struct Program *program = css_parse_file_as_stream(stream);
    fclose(stream);
    return program;
}

struct Rule* _find_first_rule(struct Program *program) {
    struct Rule *rule;
    css_iter(rule, program->rules) {
        return rule;
    }
}

START_TEST (test_css_eval_number) {
    css_eval_start();
    struct Program *program = _make_program("rule { prop: 2; }");
    struct Rule *rule = _find_first_rule(program);
    int* result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 2);

    css_free_program(program);
    css_eval_stop();
}
END_TEST

START_TEST (test_css_eval_string) {
    css_eval_start();
    struct Program *program = _make_program("rule { prop: \"foobar\"; }");
    struct Rule *rule = _find_first_rule(program);
    char* result = css_find_string_prop(rule, "prop");

    ck_assert_str_eq(result, "foobar");

    css_free_program(program);
    css_eval_stop();
}
END_TEST

START_TEST (test_css_eval_number_add) {
    css_eval_start();
    struct Program *program = _make_program("rule { prop: 2 + 2; }");
    struct Rule *rule = _find_first_rule(program);
    int* result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 4);

    css_free_program(program);
    css_eval_stop();
}
END_TEST

START_TEST (test_css_eval_number_mul_with_priority) {
    css_eval_start();
    struct Program *program = _make_program("rule { prop: 2 + 2 * 2; }");
    struct Rule *rule = _find_first_rule(program);
    int* result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 6);

    css_free_program(program);
    css_eval_stop();
}
END_TEST

START_TEST (test_css_eval_number_mul_with_braces) {
    css_eval_start();
    struct Program *program = _make_program("rule { prop: (2 + 2) * 2; }");
    struct Rule *rule = _find_first_rule(program);
    int* result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 8);

    css_free_program(program);
    css_eval_stop();
}
END_TEST

START_TEST (test_css_eval_number_choice) {
    css_eval_start();
    srand(0); // set const grain
    struct Program *program = _make_program("rule { prop: choice(1, 2); }");
    struct Rule *rule = _find_first_rule(program);
    int* result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 2);

    css_free_program(program);
    css_eval_stop();
}
END_TEST

START_TEST (test_css_eval_number_random) {
    css_eval_start();
    srand(0); // set const grain
    struct Program *program = _make_program("rule { prop: random(1, 10); }");
    struct Rule *rule = _find_first_rule(program);
    int* result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 2);

    css_free_program(program);
    css_eval_stop();
}
END_TEST

START_TEST (test_css_eval_rule_choice) {
    css_eval_start();
    srand(0); // set const grain
    struct Program *program = _make_program("rule { prop: choice(foo.foo, bar.bar); }");
    struct Rule *rule = _find_first_rule(program);
    struct RuleSelector *result = css_find_selector_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_str_eq(result->element, "bar");
    ck_assert_str_eq(result->klass, "bar");

    css_free_program(program);
    css_eval_stop();
}
END_TEST

Suite* hash_suite(void) {
    Suite *suite = suite_create("css_eval");
    TCase *tcase = tcase_create("css_eval");

    tcase_add_test(tcase, test_css_eval_number);
    tcase_add_test(tcase, test_css_eval_number_add);
    tcase_add_test(tcase, test_css_eval_number_mul_with_priority);
    tcase_add_test(tcase, test_css_eval_number_mul_with_braces);
    tcase_add_test(tcase, test_css_eval_number_choice);
    tcase_add_test(tcase, test_css_eval_number_random);
    tcase_add_test(tcase, test_css_eval_string);
    tcase_add_test(tcase, test_css_eval_rule_choice);
    suite_add_tcase(suite, tcase);
    return suite;
}

int main(void) {
    int number_failed;
    Suite *suite = hash_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
