#include "suites.h"

#include <stdlib.h>

int main(void) {
    int number_failed;
    SRunner* runner = srunner_create(hash_suite());
    srunner_add_suite(runner, css_eval_suite());

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}