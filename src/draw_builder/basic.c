#include "_draw_builder.h"
#include "css_func.h"

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

    struct BasicObj basic = {
        .width=_get_basic_metric_width(rule, 50, parent_basic),
        .height=_get_basic_metric_height(rule, 50, parent_basic),
        .depth=_get_basic_metric_depth(rule, 50, parent_basic),
    };

    return basic;
}

struct BasicObj builder_build_empty_basic() {
    struct BasicObj basic = {
        .width=0,
        .height=0,
        .depth=0,
    };

    return basic;
}

