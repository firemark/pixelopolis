#include <math.h>
#include <stdlib.h>

#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/css_func.h"

struct DrawObj* builder_build_cone(struct Helper* helper) {
    struct RuleWithParent* rule = helper->rule;
    if (!rule) return NULL;
    struct ConeObj* obj = HELPER_ALLOCATE(helper, struct ConeObj);
    struct BasicObj basic = builder_build_basic(rule, helper->parent);

    int* sides = css_find_number_prop(rule->rule, "sides");
    obj->sides = sides ? *sides : 8;

    int texture_width =
        ceil(M_PI * (3.0 / 2.0) * (basic.width + basic.depth) - sqrt(basic.width * basic.depth));

    struct SelectorHelper roof_helper = MAKE_SELECTOR_HELPER(helper, "roof");
    obj->roof = builder_build_wall(&roof_helper, texture_width, basic.height);

    return builder_make_draw_obj(helper, basic, DRAW_OBJ_CONE, obj);
}
