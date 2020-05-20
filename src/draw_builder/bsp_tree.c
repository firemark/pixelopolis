#include <stdlib.h>
#include <time.h>
#include "_draw_builder.h"
#include "css_func.h"
#include "css_eval.h"

#define RATIO_START 0.4
#define RATIO_END 0.6

enum BspTreeDirection {
    BSD_TREE_VERTICAL,
    BSD_TREE_HORIZONTAL,
};

struct BspTreeCord {
    int width, height;
    int x, y;
};

struct BspTree {
    struct BspTree* a;
    struct BspTree* b;
    struct BspTreeCord cord;
    enum BspTreeDirection direction;
};

struct BspData {
    int counter;
    int iterations_count;
};

static struct BspTree* _make_tree(
        const int iterations_count,
        const struct BspTreeCord* cord) {
    if (iterations_count < 1) return NULL;

    const double ratio = RATIO_START + (double)rand() / RAND_MAX * (RATIO_END - RATIO_START);
    struct BspTree* tree = malloc(sizeof(struct BspTree));
    tree->cord = *cord;

    tree->direction = rand() % 2 ? BSD_TREE_VERTICAL : BSD_TREE_HORIZONTAL;

    struct BspTreeCord cord_a, cord_b;

    switch(tree->direction) {
        case BSD_TREE_VERTICAL:
            cord_a.width = cord->width * ratio;
            cord_b.width = cord->width * (1.0 - ratio);

            cord_a.height = cord->height;
            cord_b.height = cord->height;

            cord_a.x = cord->x;
            cord_b.x = cord->x + cord_a.width;

            cord_a.y = cord->y;
            cord_b.y = cord->y;
        break;
        case BSD_TREE_HORIZONTAL:
            cord_a.width = cord->width;
            cord_b.width = cord->width;

            cord_a.height = cord->height * ratio;
            cord_b.height = cord->height * (1.0 - ratio);

            cord_a.x = cord->x;
            cord_b.x = cord->x;

            cord_a.y = cord->y;
            cord_b.y = cord->y + cord_a.height;
        break;
    }

    tree->a = _make_tree(iterations_count - 1, &cord_a);
    tree->b = _make_tree(iterations_count - 1, &cord_b);

    return tree;
}

static struct DrawObj* _make_child(
        struct Helper *helper,
        struct BspTree *tree,
        const int padding) {
    struct BasicObj* parent_basic = &helper->parent->basic;

    struct BasicObj basic = {
        .width=tree->cord.width - padding * 2,
        .height=parent_basic->height,
        .depth=tree->cord.height - padding * 2,
        .rotate=builder_compute_rotate(0, parent_basic),
        .v_justify=builder_get_justify(helper->rule, "justify", D_JUSTIFY),
        .d_justify=builder_get_justify(helper->rule, "justify", V_JUSTIFY),
    };
    builder_init_basic(&basic);

    struct RuleSelector* child_selector = css_find_selector_prop(helper->rule, "body");
    return builder_build_custom_void(helper, basic, child_selector);
}

static void _fill_board_from_tree(
        struct BoardObj* obj,
        struct Helper* helper,
        struct BspTree* tree,
        struct BspData* data) {
    if (tree->a || tree->b) {
        _fill_board_from_tree(obj, helper, tree->a, data);
        _fill_board_from_tree(obj, helper, tree->b, data);
        return;
    }

    const int padding = builder_get_int(helper->rule, "padding", 0);
    const int x = tree->cord.x + padding;
    const int y = tree->cord.y + padding;

    struct DrawObj* child = _make_child(helper, tree, padding);
    obj->children[data->counter++] = builder_build_board_child(child, x, y);
}

void _free_tree(struct BspTree* tree) {
    if (tree->a) _free_tree(tree->a);
    if (tree->b) _free_tree(tree->b);
    free(tree);
}

static inline const int _len_tree(const int iterations_count) {
    // power for ints
    int sum = 1;
    int i;
    for(i = 0; i < iterations_count - 1; i++) sum *= 2;
    return sum;
}

static void _fill_board(
        struct DrawObj *draw_obj,
        struct Helper* inner_helper,
        struct BspData* data) {
    const int iterations_count = data->iterations_count;
    const struct BspTreeCord cord = {
        .width=draw_obj->basic.width,
        .height=draw_obj->basic.depth,
        .x=0, .y=0,
    };
    struct BspTree* tree = _make_tree(iterations_count, &cord);

    struct BoardObj* obj = draw_obj->obj;
    const int children_len = _len_tree(iterations_count);
    obj->children = malloc(sizeof(struct BoardChild*) * (children_len + 1));
    obj->children[children_len] = NULL;

    _fill_board_from_tree(obj, inner_helper, tree, data);

    _free_tree(tree);
}

struct DrawObj* builder_build_bsp_tree(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;

    struct BoardObj* obj = malloc(sizeof(struct BoardObj));
    struct BasicObj basic = builder_build_basic(rule, helper->parent);
    struct DrawObj *draw_obj = builder_make_draw_obj(helper, basic, DRAW_OBJ_BOARD, obj);

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=helper->rule,
        .parent=draw_obj,
    };

    struct BspData data = {
        .counter=0,
        .iterations_count=builder_get_int(rule, "iterations-count", 3),
    };

    _fill_board(draw_obj, &inner_helper, &data);

    return draw_obj;
}

