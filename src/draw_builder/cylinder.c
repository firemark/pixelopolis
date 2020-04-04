#include <stdlib.h>
#include <math.h>

#include "_draw_builder.h"
#include "css_func.h"
#include "angle_iter.h"

static void _cylinder_build_many_walls(struct SelectorHelper* wall_helper, struct CylinderObj* obj, struct BasicObj* basic) {
    int width = basic->width;
    int depth = basic->depth;
    obj->walls = malloc(sizeof(struct WallObj*) * obj->sides);

    struct AngleIter angle_iter;
    angle_iter_start(&angle_iter, width, depth, obj->sides);
    while(angle_iter_iterate(&angle_iter)) {
        int length = angle_iter_get_length(&angle_iter);
        obj->walls[angle_iter.i] = builder_build_wall(wall_helper, length, basic->height);
    }
}

static void _cylinder_build_single_wall(struct SelectorHelper* wall_helper, struct CylinderObj* obj, struct BasicObj* basic) {
    int width = basic->width;
    int depth = basic->depth;
    int total_length = ceil(M_PI * (3.0 / 2.0) * (width + depth) - sqrt(width * depth)); 

    obj->walls = malloc(sizeof(struct WallObj*));
    obj->walls[0] = builder_build_wall(wall_helper, total_length, basic->height);
}

struct DrawObj* builder_build_cylinder(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct CylinderObj* obj = malloc(sizeof(struct CylinderObj));
    struct BasicObj basic = builder_build_basic(rule, helper->parent);

    obj->has_many_walls = builder_get_int(rule, "has-many-walls", 0);
    obj->sides = builder_get_int(rule, "sides", 8);

    struct SelectorHelper wall_helper = make_selector_helper(helper, "wall");
    if (!wall_helper.selector) {
        obj->walls = NULL;
    } else {
        if (obj->has_many_walls) {
            _cylinder_build_many_walls(&wall_helper, obj, &basic);
        } else {
            _cylinder_build_single_wall(&wall_helper, obj, &basic);
        } 
    }

    struct SelectorHelper roof_helper = make_selector_helper(helper, "roof");
    obj->roof = builder_build_wall(&roof_helper, basic.width, basic.depth);

    return builder_make_draw_obj(helper, basic, DRAW_OBJ_CYLINDER, obj);
}
