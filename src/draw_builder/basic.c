#include <math.h>

#include "_draw_builder.h"
#include "css_func.h"


#define DEFAULT_METRIC 0

#define MAKE_GET_METRIC(NAME, ATTR, PARENT_TYPE) \
int NAME ## _ ## ATTR(struct Rule* rule, int default_value, PARENT_TYPE* parent) { \
    struct Obj* obj = css_find_1st_obj(rule, #ATTR); \
    if (!obj) return default_value; \
    if (obj->type == OBJ_NUMBER) return *(int*)obj->value; \
    if (obj->type != OBJ_PERCENT) return default_value; \
    if (!parent) return default_value; \
    int percent = *(int*)obj->value; \
    int parent_value = parent->ATTR; \
    return (percent * parent_value) / 100; \
}
MAKE_GET_METRIC(_get_basic_metric, width, struct BasicObj)
MAKE_GET_METRIC(_get_basic_metric, height, struct BasicObj)
MAKE_GET_METRIC(_get_basic_metric, depth, struct BasicObj)

struct BasicObj builder_build_basic(struct Rule* rule, struct DrawObj* parent) {
    struct BasicObj* parent_basic = parent ? &parent->basic : NULL;
    const int primal_rotate = builder_get_int(rule, "rotate", 0);
    const int rotate = builder_compute_rotate(primal_rotate, parent_basic);
    const int width = _get_basic_metric_width(rule, DEFAULT_METRIC, parent_basic);
    const int height = _get_basic_metric_height(rule, DEFAULT_METRIC, parent_basic);
    const int depth = _get_basic_metric_depth(rule, DEFAULT_METRIC, parent_basic);

    return builder_build_custom_basic(width, height, depth, rotate);
}

struct BasicObj builder_build_custom_basic(
        const int width, 
        const int height, 
        const int depth,
        const int rotate) {
    const double theta = rotate * M_PI / 180.0;
    return (struct BasicObj) {
        .width=width,
        .height=height,
        .depth=depth,
        .rotate=rotate,
        .sin_th=sin(theta),
        .cos_th=cos(theta),
    };
}

struct BasicObj builder_build_empty_basic() {
    struct BasicObj basic = {
        .width=0,
        .height=0,
        .depth=0,
        .rotate=0,
        .sin_th=0.0,
        .cos_th=1.0,
    };

    return basic;
}

