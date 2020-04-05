#include <stdlib.h>
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

int builder_get_padding(struct Rule* rule) {
    return builder_get_int(rule, "padding", 0);
}

int builder_compute_rotate(const int child_rotate, struct BasicObj* parent_basic) {
    int parent_rotate = parent_basic ? parent_basic->rotate : 0;
    return (child_rotate + parent_rotate) % 360;
}
