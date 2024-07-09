#include <string.h>

#include "pixelopolis/_draw_builder_utils.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/draw_builder_common.h"

int builder_get_int(struct Rule* rule, char* key, const int default_value) {
    int* ptr = css_find_number_prop(rule, key);
    return ptr ? *ptr : default_value;
}

int builder_get_percent(struct Rule* rule, char* key, const int default_value) {
    int* ptr = css_find_percent_prop(rule, key);
    return ptr ? *ptr : default_value;
}

int builder_get_padding(struct Rule* rule) { return builder_get_int(rule, "padding", 0); }

static inline size_t _get_size(struct Obj** prop_objs) {
    size_t size = 0;
    struct Obj* obj = NULL;
    css_iter (obj, prop_objs) {
        size++;  // counter
    }
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