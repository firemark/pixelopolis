#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"

#define SET_GREATER(attr) a->attr = a->attr >= b->attr ? a->attr : b->attr

struct Rule* builder_make_rule_from_helper(struct SelectorHelper* helper) {
    if (!helper->selector) return NULL;
    struct RuleSelector* selector = css_cpy_selector(helper->selector);
    selector->parent = helper->parent_rule ? helper->parent_rule->selector : NULL;
    return css_make_rule_from_selector(helper->program, selector);
}

int builder_get_basic_metric_by_fill_direction(struct BasicObj *basic, enum FillDirection fill_direction) {
    switch(fill_direction) {
        case VERTICAL_FILL: return basic->width;
        case HORIZONTAL_FILL: return basic->height;
        case DEPTH_FILL: return basic->depth;
        default: return 0;
    }
}

void builder_add_basic_metric_by_fill_direction(struct BasicObj *basic, enum FillDirection fill_direction, int x) {
    switch(fill_direction) {
        case VERTICAL_FILL: basic->width += x; break;
        case HORIZONTAL_FILL: basic->height += x; break;
        case DEPTH_FILL: basic->depth += x; break;
    }
}

void builder_add_max_basic_by_fill_direction(struct BasicObj *a, struct BasicObj *b, enum FillDirection fill_direction) {
    switch(fill_direction) {
        case VERTICAL_FILL: 
            a->width += b->width;
            SET_GREATER(height);
            SET_GREATER(depth);
            break;
        case HORIZONTAL_FILL: 
            a->height += b->height;
            SET_GREATER(width);
            SET_GREATER(depth);
            break;
        case DEPTH_FILL: 
            a->depth += a->depth;
            SET_GREATER(width);
            SET_GREATER(height);
            break;
    }
}

void builder_max_basic(struct BasicObj *a, struct BasicObj *b) {
    SET_GREATER(width);
    SET_GREATER(height);
    SET_GREATER(depth);
}

struct DrawObj* builder_make_draw_obj(struct Helper* helper, struct BasicObj basic, enum DrawObjType type, void* obj) {
    struct DrawObj* draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->basic = basic;
    draw_obj->type = type;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    return draw_obj;
}
