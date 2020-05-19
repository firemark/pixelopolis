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
    struct BasicObj basic = {
        .width=_get_basic_metric_width(rule, DEFAULT_METRIC, parent_basic),
        .height=_get_basic_metric_height(rule, DEFAULT_METRIC, parent_basic),
        .depth=_get_basic_metric_depth(rule, DEFAULT_METRIC, parent_basic),
        .rotate=builder_compute_rotate(primal_rotate, parent_basic),
        .v_justify=builder_get_justify(rule, "justify", V_JUSTIFY),
        .d_justify=builder_get_justify(rule, "justify", D_JUSTIFY),
    };
    builder_init_basic(&basic);

    return basic;
}

void builder_init_basic(struct BasicObj* basic) {
    const double theta = builder_make_theta(basic->rotate);
    basic->sin_th = sin(theta);
    basic->cos_th = cos(theta);
}


struct BasicObj builder_build_empty_basic() {
    return (struct BasicObj) {
        .width=0,
        .height=0,
        .depth=0,
        .rotate=0,
        .sin_th=0.0, // sin(0.0)
        .cos_th=1.0, // cos(0.0)
        .v_justify=JUSTIFY_START,
        .d_justify=JUSTIFY_START,
    };
}

