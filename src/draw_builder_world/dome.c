#include <math.h>
#include <stdlib.h>

#include "pixelopolis/_draw_builder_world.h"
#include "pixelopolis/css/func.h"

struct DrawObj* builder_build_dome(struct Helper* helper) {
    struct RuleWithParent* rule = helper->rule;
    if (!rule) return NULL;
    struct DomeObj* obj = HELPER_ALLOCATE(helper, struct DomeObj);
    struct BasicObj basic = builder_build_basic(rule, helper->parent);

    obj->vertical_sides = builder_get_int(rule, "vertical-sides", 8);
    obj->horizontal_sides = builder_get_int(rule, "horizontal-sides", 8);

    int texture_width =
        ceil(M_PI * (3.0 / 2.0) * (basic.width + basic.depth) - sqrt(basic.width * basic.depth));

    struct SelectorHelper roof_helper = MAKE_SELECTOR_HELPER(helper, "roof");
    obj->roof = builder_build_wall(&roof_helper, texture_width, basic.height);

    return builder_make_draw_obj(helper, basic, DRAW_OBJ_DOME, obj);
}
