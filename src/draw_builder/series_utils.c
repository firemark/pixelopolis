#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"

#define SET_GREATER(attr) a->attr = a->attr >= b->attr ? a->attr : b->attr

void series_add_basic_metric_by_fill_direction(struct BasicObj *basic, enum FillDirection fill_direction, int x) {
    switch(fill_direction) {
        case VERTICAL_FILL: basic->width += x; break;
        case HORIZONTAL_FILL: basic->height += x; break;
        case DEPTH_FILL: basic->depth += x; break;
    }
}

int series_get_basic_metric_by_fill_direction(struct BasicObj *basic, enum FillDirection fill_direction) {
    switch(fill_direction) {
        case VERTICAL_FILL: return basic->width;
        case HORIZONTAL_FILL: return basic->height;
        case DEPTH_FILL: return basic->depth;
        default: return 0;
    }
}

void series_add_max_basic_by_fill_direction(struct BasicObj *a, struct BasicObj *b, enum FillDirection fill_direction) {
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
            a->depth += b->depth;
            SET_GREATER(width);
            SET_GREATER(height);
            break;
    }
}

void series_max_basic(struct BasicObj *a, struct BasicObj *b) {
    SET_GREATER(width);
    SET_GREATER(height);
    SET_GREATER(depth);
}

struct ShiftDrawPair* series_make_pair(int shift, struct DrawObj* obj) {
    struct ShiftDrawPair *pair = malloc(sizeof(struct ShiftDrawPair));
    pair->obj = obj;
    pair->shift = shift;
    return pair;
}

struct DrawObj* series_make_draw_obj(struct Helper* helper, struct RuleSelector* selector) {
    if (!selector) return NULL;
    struct SelectorHelper inner_helper = {
        .program=helper->program,
        .parent_rule=helper->rule,
        .selector=selector,
        .parent=helper->parent,
    };
    struct DrawObj *child = builder_build_draw_obj(&inner_helper);
    if (!child) return NULL;
    return child;
}
