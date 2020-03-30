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

int _builder_build_basic_get_rotate(
        struct Rule* rule,
        struct BasicObj* parent_basic) {
    int* p_rotate = css_find_number_prop(rule, "rotate");
    int child_rotate = p_rotate? *p_rotate : 0;
    int parent_rotate = parent_basic ? parent_basic->rotate : 0;
    return (child_rotate + parent_rotate) % 360;
}

struct BasicObj builder_build_basic(struct Rule* rule, struct DrawObj* parent) {
    struct BasicObj* parent_basic = parent ? &parent->basic : NULL;

    struct BasicObj basic = {
        .width=_get_basic_metric_width(rule, DEFAULT_METRIC, parent_basic),
        .height=_get_basic_metric_height(rule, DEFAULT_METRIC, parent_basic),
        .depth=_get_basic_metric_depth(rule, DEFAULT_METRIC, parent_basic),
        .rotate=_builder_build_basic_get_rotate(rule, parent_basic),
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

