#include <stdlib.h>

#include "pixelopolis/_draw_builder_world.h"
#include "pixelopolis/css/func.h"

struct DrawObj* builder_build_cube(struct Helper* helper) {
    struct RuleWithParent* rule = helper->rule;
    if (!rule) return NULL;
    struct CubeObj* obj = HELPER_ALLOCATE(helper, struct CubeObj);

    struct BasicObj basic = builder_build_basic(rule, helper->parent);

    struct SelectorHelper wall_helper = MAKE_SELECTOR_HELPER(helper, "wall");
    obj->south_wall = builder_build_wall(&wall_helper, basic.width, basic.height);
    obj->east_wall = builder_build_wall(&wall_helper, basic.depth, basic.height);

    struct SelectorHelper roof_helper = MAKE_SELECTOR_HELPER(helper, "roof");
    obj->roof = builder_build_wall(&roof_helper, basic.width, basic.depth);

    return builder_make_draw_obj(helper, basic, DRAW_OBJ_CUBE, obj);
}
