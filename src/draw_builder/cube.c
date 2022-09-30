#include <stdlib.h>
#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/css_func.h"

struct DrawObj* builder_build_cube(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct CubeObj* obj = malloc(sizeof(struct CubeObj));
    struct BasicObj basic = builder_build_basic(rule, helper->parent);

    struct SelectorHelper wall_helper = make_selector_helper(helper, "wall");
    obj->south_wall = builder_build_wall(&wall_helper, basic.width, basic.height);
    obj->east_wall = builder_build_wall(&wall_helper, basic.depth, basic.height);

    struct SelectorHelper roof_helper = make_selector_helper(helper, "roof");
    obj->roof = builder_build_wall(&roof_helper, basic.width, basic.depth);

    return builder_make_draw_obj(helper, basic, DRAW_OBJ_CUBE, obj);
}
