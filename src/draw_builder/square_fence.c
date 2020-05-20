#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"
#include "css_eval.h"

#define EDGES_COUNT 4
#define BODY_INDEX (0)
#define EDGE_INDEX (BODY_INDEX + 1)
#define CORNER_INDEX (EDGE_INDEX + EDGES_COUNT)

struct SquareFenceData {
    int edge_size;
};

static void _fill_body(
        struct BoardObj* obj,
        struct Helper *helper,
        const struct SquareFenceData *data) {
    struct BasicObj* parent_basic = &helper->parent->basic;
    const int edge_size = data->edge_size;
    const int double_edge_size = edge_size * 2;
    struct BasicObj basic = {
        .width=parent_basic->width - double_edge_size,
        .height=parent_basic->height,
        .depth=parent_basic->depth - double_edge_size,
        .rotate=builder_compute_rotate(0, parent_basic),
        .v_justify=builder_get_justify(helper->rule, "body-justify", V_JUSTIFY),
        .d_justify=builder_get_justify(helper->rule, "body-justify", D_JUSTIFY),
    };
    builder_init_basic(&basic);
    struct RuleSelector* child_selector = css_find_selector_prop(helper->rule, "body");

    obj->children[BODY_INDEX] = builder_build_board_child(
        builder_build_custom_void(helper, basic, child_selector),
        edge_size, edge_size);
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
        .v_justify=builder_get_justify(helper->rule, "corner-justify", V_JUSTIFY),
        .d_justify=builder_get_justify(helper->rule, "corner-justify", D_JUSTIFY),
    };
    builder_init_basic(&basic);
    struct RuleSelector* child_selector = css_find_selector_prop(helper->rule, "corner");

    return builder_build_custom_void(helper, basic, child_selector);
}

static struct DrawObj* _make_edge(
        struct Helper *helper,
        const char is_vertical,
        const int primal_rotate,
        const struct SquareFenceData* data) {
    struct BasicObj* parent_basic = &helper->parent->basic;
    const int edge_size = data->edge_size;
    const int primal_width = is_vertical ? parent_basic->width : parent_basic->depth;

    struct BasicObj basic = {
        .width=primal_width - edge_size * 2,
        .height=parent_basic->height,
        .depth=edge_size,
        .rotate=builder_compute_rotate(primal_rotate, parent_basic),
        .v_justify=builder_get_justify(helper->rule, "edge-justify", D_JUSTIFY),
        .d_justify=builder_get_justify(helper->rule, "edge-justify", V_JUSTIFY),
    };
    builder_init_basic(&basic);
    struct RuleSelector* child_selector = css_find_selector_prop(helper->rule, "edge");
    return builder_build_custom_void(helper, basic, child_selector);
}

static void _fill_edges_and_corners(
        struct BoardObj* obj,
        struct Helper *helper,
        const struct SquareFenceData *data) {
    struct BoardChild** children = obj->children;
    const int edge_size = data->edge_size;
    int edge_x = 0, edge_y = 0;
    int corner_x = 0, corner_y = 0;

    int i;
    for(i = 0; i < EDGES_COUNT; i++) {
        const int rotate = 90 * i;
        const double theta = builder_make_theta(rotate);
        const double sin_th = sin(theta);
        const double cos_th = cos(theta);

        const char is_vertical = (i % 2) == 0;

        struct DrawObj* corner = _make_corner(helper, data->edge_size);
        children[CORNER_INDEX + i] = builder_build_board_child(corner, corner_x, corner_y);

        edge_x += edge_size * cos_th;
        edge_y += edge_size * sin_th;

        struct DrawObj* edge = _make_edge(helper, is_vertical, rotate, data);
        children[EDGE_INDEX + i] = builder_build_board_child(edge, edge_x, edge_y);

        const int len = edge->basic.width + edge_size;
        const int dx = len * cos_th;
        const int dy = len * sin_th;

        edge_x += dx;
        edge_y += dy;
        corner_x += dx;
        corner_y += dy;
    }
}

struct DrawObj* builder_build_square_fence(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;

    struct BoardObj* obj = malloc(sizeof(struct BoardObj));
    const int children_len = 1 + EDGES_COUNT * 2 + 1; // body + X corners + X edges + NULL
    obj->children = malloc(sizeof(struct BoardChild*) * children_len);
    obj->children[children_len - 1] = NULL;
    struct BasicObj basic = builder_build_basic(rule, helper->parent);
    struct DrawObj *draw_obj = builder_make_draw_obj(helper, basic, DRAW_OBJ_BOARD, obj);

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=helper->rule,
        .parent=draw_obj,
    };

    const struct SquareFenceData data = {
        .edge_size=builder_get_int(rule, "edge-size", 25),
    };

    _fill_body(obj, &inner_helper, &data);
    _fill_edges_and_corners(obj, &inner_helper, &data);

    return draw_obj;
}

