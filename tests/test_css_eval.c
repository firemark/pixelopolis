#include "suites.h"

#include <stdio.h>
#include <stdlib.h>

#include "pixelopolis/css/eval.h"
#include "pixelopolis/css/func.h"

static struct Program *program;

void setup(void) {
    css_eval_start();
    program = NULL;
    srand(0);  // set const grain
}

void teardown(void) {
    css_eval_stop();
    if(program) {
      css_free_program(program);
    }
}

static void _make_program(char *buffer) {
    FILE *stream = fmemopen(buffer, strlen(buffer), "r");
    program = css_parse_file_as_stream(stream);
    fclose(stream);
}

static struct Rule *_find_first_rule() {
    struct Rule *rule;
    css_iter (rule, program->rules) { return rule; }
}

START_TEST(test_css_eval_number) {
    _make_program("rule { prop: 2; }");
    struct Rule *rule = _find_first_rule();
    int *result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 2);
}
END_TEST

START_TEST(test_css_eval_string) {
    _make_program("rule { prop: \"foobar\"; }");
    struct Rule *rule = _find_first_rule();
    char *result = css_find_string_prop(rule, "prop");

    ck_assert_str_eq(result, "foobar");
}
END_TEST

START_TEST(test_css_eval_number_add) {
    _make_program("rule { prop: 2 + 2; }");
    struct Rule *rule = _find_first_rule();
    int *result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 4);
}
END_TEST

START_TEST(test_css_eval_number_mul_with_priority) {
    _make_program("rule { prop: 2 + 2 * 2; }");
    struct Rule *rule = _find_first_rule();
    int *result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 6);
}
END_TEST

START_TEST(test_css_eval_number_mul_with_braces) {
    _make_program("rule { prop: (2 + 2) * 2; }");
    struct Rule *rule = _find_first_rule();
    int *result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 8);
}
END_TEST

START_TEST(test_css_eval_number_choice) {
    _make_program("rule { prop: choice(1, 2); }");
    struct Rule *rule = _find_first_rule();
    int *result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 2);
}
END_TEST

START_TEST(test_css_eval_number_random) {
    _make_program("rule { prop: random(1, 10); }");
    struct Rule *rule = _find_first_rule();
    int *result = css_find_number_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 2);
}
END_TEST

START_TEST(test_css_eval_rule_choice) {
    _make_program("rule { prop: choice(foo.foo, bar.bar); }");
    struct Rule *rule = _find_first_rule();
    struct RuleSelector *result = css_find_selector_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_str_eq(result->element, "bar");
    // ck_assert_str_eq(result->klasses, "bar");
}
END_TEST

START_TEST(test_css_eval_percent) {
    _make_program("rule { prop: 100%; }");
    struct Rule *rule = _find_first_rule();
    int *result = css_find_percent_prop(rule, "prop");

    ck_assert_ptr_ne(result, NULL);
    ck_assert_int_eq(*result, 100);
}
END_TEST

Suite* css_eval_suite(void) {
    Suite *suite = suite_create("css_eval");
    TCase *tcase = tcase_create("css_eval");

    tcase_add_test(tcase, test_css_eval_number);
    tcase_add_test(tcase, test_css_eval_number_add);
    tcase_add_test(tcase, test_css_eval_number_mul_with_priority);
    tcase_add_test(tcase, test_css_eval_number_mul_with_braces);
    tcase_add_test(tcase, test_css_eval_number_choice);
    tcase_add_test(tcase, test_css_eval_number_random);
    tcase_add_test(tcase, test_css_eval_string);
    tcase_add_test(tcase, test_css_eval_percent);
    tcase_add_test(tcase, test_css_eval_rule_choice);
    tcase_add_checked_fixture(tcase, setup, teardown);
    suite_add_tcase(suite, tcase);
    return suite;
}