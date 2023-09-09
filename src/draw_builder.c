#include <string.h>

#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/img.h"

void builder_init(void) {}

void builder_stop(void) {}

struct DrawObj* builder_make(struct Program* program, struct Rule* world) {
    struct RuleSelector* query = css_find_selector_prop(world, "body");
    struct SelectorHelper helper = {
        .program = program,
        .parent_rule = NULL,
        .selector = query,
        .parent = NULL,
    };
    return builder_build_draw_obj(&helper);
}
