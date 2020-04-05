#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"
#include "css_eval.h"

#define EDGES_COUNT 4

static void _fill_body(struct SquareFenceObj* obj, struct Helper *helper) {
    struct BasicObj* parent_basic = &helper->parent->basic;
    const int edge_size = obj->edge_size * 2;
    struct BasicObj basic = {
        .width=parent_basic->width - edge_size,
        .height=parent_basic->height,
        .depth=parent_basic->depth - edge_size,
        .rotate=builder_compute_rotate(0, parent_basic),
    };
    builder_init_basic(&basic);
    struct RuleSelector* child_selector = css_find_selector_prop(helper->rule, "body");

    obj->body = builder_build_custom_void(helper, basic, child_selector);
}

static struct DrawObj* _make_corner(
        struct Helper *helper,
        const int size) {
    struct BasicObj* parent_basic = &helper->parent->basic;
    struct BasicObj basic = {
        .width=size,
        .height=parent_basic->height,
        .depth=size,
        .rotate=builder_compute_rotate(0, parent_basic),
    };
    builder_init_basic(&basic);
    struct RuleSelector* child_selector = css_find_selector_prop(helper->rule, "corner");

    return builder_build_custom_void(helper, basic, child_selector);
}

static void _fill_corners(struct SquareFenceObj* obj, struct Helper *helper) {
    obj->corners = malloc(sizeof(struct DrawObj*) * 4);
    int i;
    for(i = 0; i < EDGES_COUNT; i++) {
        obj->corners[i] = _make_corner(helper, obj->edge_size);
    }
}

static struct DrawObj* _make_edge(
        struct Helper *helper,
        struct SquareFenceObj* obj,
        const char is_vertical,
        const int primal_rotate) {
    struct BasicObj* parent_basic = &helper->parent->basic;
    const int edge_size = obj->edge_size;
    const int primal_width = is_vertical ? parent_basic->width : parent_basic->depth;

    struct BasicObj basic = {
        .width=primal_width - edge_size * 2,
        .height=parent_basic->height,
        .depth=edge_size,
        .rotate=builder_compute_rotate(primal_rotate, parent_basic),
    };
    builder_init_basic(&basic);
    struct RuleSelector* child_selector = css_find_selector_prop(helper->rule, "edge");
    return builder_build_custom_void(helper, basic, child_selector);
}

static void _fill_edges(struct SquareFenceObj* obj, struct Helper *helper) {
    obj->edges = malloc(sizeof(struct DrawObj*) * 4);
    int i;
    for(i = 0; i < EDGES_COUNT; i++) {
        const int rotate = 90 * i;
        const char is_vertical = (i % 2) == 0;
        obj->edges[i] = _make_edge(helper, obj, is_vertical, rotate);
    }
}

struct DrawObj* builder_build_square_fence(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;

    struct SquareFenceObj* obj = malloc(sizeof(struct SquareFenceObj));
    obj->edge_size = builder_get_int(rule, "edge-size", 25);

    struct BasicObj basic = builder_build_basic(rule, helper->parent);
    struct DrawObj *draw_obj = builder_make_draw_obj(helper, basic, DRAW_OBJ_SQUARE_FENCE, obj);

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=helper->rule,
        .parent=draw_obj,
    };

    _fill_body(obj, &inner_helper);
    _fill_corners(obj, &inner_helper);
    _fill_edges(obj, &inner_helper);

    return draw_obj;
}

