#include <stdlib.h>
#include <time.h>
#include "_draw_builder.h"
#include "css_func.h"
#include "css_eval.h"

#define RATIO_START 0.3
#define RATIO_END 0.7

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
    int real_width, real_height;
    int padding;
    enum BspTreeDirection direction;
};

struct BspData {
    int counter;
    int iterations_count;
};

static inline enum BspTreeDirection _get_direction(
        const char is_greater_width,
        const char is_greater_height) {
    if (is_greater_width && !is_greater_height) {
        return BSD_TREE_VERTICAL;
    }

    if (!is_greater_width && is_greater_height) {
        return BSD_TREE_HORIZONTAL;
    }

    return rand() % 2 ? BSD_TREE_VERTICAL : BSD_TREE_HORIZONTAL;
}

static void _make_branches(
        struct BspTree* tree,
        const int iterations_count,
        const struct Helper* helper,
        const int max_width,
        const int max_height);

static struct BspTree* _make_tree(
        const int iterations_count,
        const struct BspTreeCord* cord,
        const struct Helper* helper) {
    const int padding = builder_get_int(helper->rule, "padding", 0);
    const int double_padding = padding * 2;
    const int real_width = cord->width - double_padding;
    const int real_height = cord->height - double_padding;
    const int min_width = builder_get_int(helper->rule, "min-width", -1);
    const int min_height = builder_get_int(helper->rule, "min-height", -1);
    if (real_width < min_width
        || real_height < min_height) return NULL;

    const int max_width = builder_get_int(helper->rule, "max-width", 0);
    const int max_height = builder_get_int(helper->rule, "max-height", 0);
    const char is_greater_width = max_width > 0 && real_width > max_width;
    const char is_greater_height = max_height > 0 && real_height > max_height;
    if (!is_greater_width
        && !is_greater_height
        && iterations_count < 1) return NULL;

    struct BspTree* tree = malloc(sizeof(struct BspTree));
    tree->cord = *cord;
    tree->direction = _get_direction(is_greater_width, is_greater_height);
    tree->padding = padding;
    tree->real_width = real_width;
    tree->real_height = real_height;

    _make_branches(tree, iterations_count, helper, max_width, max_height);

    return tree;
}

static void _make_branches(
        struct BspTree* tree,
        const int iterations_count,
        const struct Helper* helper,
        const int max_width,
        const int max_height) {
    struct BspTreeCord *cord = &tree->cord;
    struct BspTreeCord cord_a, cord_b;
    int min_width, min_height;
    const double ratio = RATIO_START + (double)rand() / RAND_MAX * (RATIO_END - RATIO_START);

    tree->a = NULL;
    tree->b = NULL;

    switch(tree->direction) {
        case BSD_TREE_VERTICAL:
            min_width = builder_get_int(helper->rule, "min-width", 0);
            min_width = min_width < max_width ? min_width : max_width;
            cord_a.width = cord->width * ratio;
            cord_b.width = cord->width * (1.0 - ratio);

            if (cord_a.width < min_width || cord_b.width < min_width) {
                return;
            }

            cord_a.height = cord->height;
            cord_b.height = cord->height;

            cord_a.x = cord->x;
            cord_b.x = cord->x + cord_a.width;

            cord_a.y = cord->y;
            cord_b.y = cord->y;
        break;
        case BSD_TREE_HORIZONTAL:
            min_height = builder_get_int(helper->rule, "min-height", 0);
            min_height = min_height < max_height ? min_height : max_height;
            cord_a.height = cord->height * ratio;
            cord_b.height = cord->height * (1.0 - ratio);

            if (cord_a.height < min_height || cord_b.height < min_height) {
                return;
            }

            cord_a.width = cord->width;
            cord_b.width = cord->width;

            cord_a.x = cord->x;
            cord_b.x = cord->x;

            cord_a.y = cord->y;
            cord_b.y = cord->y + cord_a.height;
        break;
    }

    tree->a = _make_tree(iterations_count - 1, &cord_a, helper);
    tree->b = _make_tree(iterations_count - 1, &cord_b, helper);
}

static struct DrawObj* _make_child(
        struct Helper *helper,
        struct BspTree *tree) {
    struct BasicObj* parent_basic = &helper->parent->basic;

    struct BasicObj basic = {
        .width=tree->real_width,
        .height=parent_basic->height,
        .depth=tree->real_height,
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
    if (!tree) return;
    if (tree->a || tree->b) {
        _fill_board_from_tree(obj, helper, tree->a, data);
        _fill_board_from_tree(obj, helper, tree->b, data);
        return;
    }

    const int padding = tree->padding;
    const int x = tree->cord.x + padding;
    const int y = tree->cord.y + padding;

    struct DrawObj* child = _make_child(helper, tree);
    obj->children[data->counter++] = builder_build_board_child(child, x, y);
}

void _free_tree(struct BspTree* tree) {
    if (!tree) return;
    if (tree->a) _free_tree(tree->a);
    if (tree->b) _free_tree(tree->b);
    free(tree);
}

static inline const int _len_tree(struct BspTree* tree) {
    if (!tree) return 0;
    if (!tree->a && !tree->b) return 1;
    return _len_tree(tree->a) + _len_tree(tree->b);
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
    struct BspTree* tree = _make_tree(iterations_count, &cord, inner_helper);

    struct BoardObj* obj = draw_obj->obj;
    const int children_len = _len_tree(tree);
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

