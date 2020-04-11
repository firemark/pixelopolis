#include <stdlib.h>
#include <string.h>

#include "_draw_builder.h"
#include "css_func.h"

struct Rule* builder_make_rule_from_helper(struct SelectorHelper* helper) {
    if (!helper->selector) return NULL;
    struct RuleSelector* selector = css_cpy_selector(helper->selector);
    selector->parent = helper->parent_rule ? helper->parent_rule->selector : NULL;
    return css_make_rule_from_selector(helper->program, selector);
}


struct DrawObj* builder_make_draw_obj(struct Helper* helper, struct BasicObj basic, enum DrawObjType type, void* obj) {
    struct DrawObj* draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->basic = basic;
    draw_obj->type = type;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    return draw_obj;
}

int builder_get_int(struct Rule* rule, char* key, const int default_value) {
    int* ptr = css_find_number_prop(rule, key);
    return ptr ? *ptr : default_value;
}

int builder_get_percent(struct Rule* rule, char* key, const int default_value) {
    int* ptr = css_find_percent_prop(rule, key);
    return ptr ? *ptr : default_value;
}

int builder_get_padding(struct Rule* rule) {
    return builder_get_int(rule, "padding", 0);
}

int builder_compute_rotate(const int child_rotate, struct BasicObj* parent_basic) {
    int parent_rotate = parent_basic ? parent_basic->rotate : 0;
    return (child_rotate + parent_rotate) % 360;
}

static inline size_t _get_size(struct Obj** prop_objs) {
    size_t size = 0;
    struct Obj* obj = NULL;
    css_iter(obj, prop_objs) size++; // counter
    return size;
}

static inline char* _builder_get_name_justify(const struct Obj* obj) {
    if (!obj || obj->type != OBJ_RULE) return NULL;
    const struct RuleSelector* selector = obj->value;
    return selector->element;
}

static inline enum Justify _builder_get_main_justify(const char* name) {
#define IF_NAME(s) else if (!strcmp(name, s))
    if (!name) return JUSTIFY_START;
    IF_NAME("center") return JUSTIFY_CENTER;
    IF_NAME("random") return JUSTIFY_RANDOM;
#undef IF_NAME
    return JUSTIFY_START;
}

static inline enum Justify _builder_get_single_justify(const char* name) {
#define IF_NAME(s) else if (!strcmp(name, s))
    if (!name) return JUSTIFY_START;
    IF_NAME("start") return JUSTIFY_START;
    IF_NAME("center") return JUSTIFY_CENTER;
    IF_NAME("end") return JUSTIFY_END;
    IF_NAME("random") return JUSTIFY_RANDOM;
#undef IF_NAME
    return JUSTIFY_START;
}

enum Justify builder_get_justify(struct Rule* rule, char* key, const enum JustifyIndex index) {
    struct Obj** prop_objs = css_find_objs(rule, key);
    if (!prop_objs) {
        return JUSTIFY_START;
    }
    const size_t size = _get_size(prop_objs);

    if (size == 0) {
        return JUSTIFY_START;
    }

    if (size <= index) {
        // check if justify has a global parameter when is only 1 element
        const char* name = _builder_get_name_justify(prop_objs[0]);
        return _builder_get_main_justify(name);
    }

    const char* name = _builder_get_name_justify(prop_objs[index]);
    return _builder_get_single_justify(name);
}
