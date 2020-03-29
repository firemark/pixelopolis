#include <stdlib.h>
#include <math.h>

#include "_draw_builder.h"
#include "css_func.h"

struct DrawObj* builder_build_dome(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct DomeObj* obj = malloc(sizeof(struct ConeObj));
    struct BasicObj basic = builder_build_basic(rule, helper->parent);

    int* vertical_sides = css_find_number_prop(rule, "vertical-sides");
    obj->vertical_sides = vertical_sides ? *vertical_sides : 8;

    int* horizontal_sides = css_find_number_prop(rule, "horizontal-sides");
    obj->horizontal_sides = horizontal_sides ? *horizontal_sides : 8;

    int texture_width = ceil(
        M_PI * (3.0 / 2.0) * (basic.width + basic.depth) 
        - sqrt(basic.width * basic.depth)
    ); 

    struct SelectorHelper roof_helper = make_selector_helper(helper, "roof");
    obj->roof = builder_build_wall(&roof_helper, texture_width, basic.height);

    return builder_make_draw_obj(helper, basic, DRAW_OBJ_DOME, obj);
}
