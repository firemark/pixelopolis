#include <stdlib.h>

#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/css_eval.h"
#include "pixelopolis/css_func.h"

static inline const size_t _get_size(struct Obj** prop_objs) {
    size_t size = 0;
    struct Obj* obj = NULL;
    css_iter (obj, prop_objs)
        size++;  // counter
    return size;
}

static struct BoardChild* _make_child(struct RuleSelector* child_selector, struct Helper* helper) {
    struct BasicObj* parent_basic = &helper->parent->basic;
    struct BasicObj basic = {
        .width = parent_basic->width,
        .height = parent_basic->height,
        .depth = parent_basic->depth,
        .rotate = builder_compute_rotate(0, parent_basic),
        .v_justify = builder_get_justify(helper->rule, "justify", V_JUSTIFY),
        .d_justify = builder_get_justify(helper->rule, "justify", D_JUSTIFY),
    };
    builder_init_basic(&basic);

    struct DrawObj* child = builder_build_custom_void(helper, basic, child_selector);
    return builder_build_board_child(helper, child, 0, 0);
}

static void _fill(struct BoardObj* board_obj, struct Obj** prop_objs, struct Helper* helper) {
    const size_t children_len = _get_size(prop_objs);
    board_obj->children = HELPER_ALLOCATE_ARRAY(helper, struct BoardChild*, children_len + 1);

    size_t index = 0;
    struct Obj* obj = NULL;
    css_iter (obj, prop_objs) {
        struct RuleSelector* child_selector = css_eval_rule(obj);
        if (!child_selector) continue;
        board_obj->children[index++] = _make_child(child_selector, helper);
    }

    board_obj->children[index] = NULL;
}

struct DrawObj* builder_build_union(struct Helper* helper) {
    struct RuleWithParent* rule = helper->rule;
    if (!rule) return NULL;

    struct Obj** prop_objs = css_find_objs(rule->rule, "body");
    if (!prop_objs) {
        return NULL;
    }

    struct BoardObj* obj = HELPER_ALLOCATE(helper, struct BoardObj);
    struct BasicObj basic = builder_build_basic(rule, helper->parent);
    struct DrawObj* draw_obj = builder_make_draw_obj(helper, basic, DRAW_OBJ_BOARD, obj);

    struct Helper inner_helper = {
        .program = helper->program,
        .rule = helper->rule,
        .parent = draw_obj,
    };

    _fill(obj, prop_objs, &inner_helper);

    return draw_obj;
}
