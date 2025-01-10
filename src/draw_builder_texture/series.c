#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/_draw_builder_texture_part.h"
#include "pixelopolis/css/eval.h"
#include "pixelopolis/css/func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/draw_builder_texture.h"

static struct ShiftTexPair** _append_children(struct Helper* helper, struct BasicTexObj* basic,
                                              enum TexPartDirection direction);

struct TexObj* builder_texture_build_texture_series(struct Helper* helper,
                                                    enum TexPartDirection direction) {
    struct TexPartObj* obj = HELPER_ALLOCATE(helper, struct TexPartObj);
    struct BasicTexObj basic = builder_texture_prepare_basic(helper);

    {
        struct SelectorHelper background_helper = MAKE_SELECTOR_HELPER(helper, "background");
        obj->background = builder_texture_build_tex_obj(&background_helper);
    }
    obj->objs = _append_children(helper, &basic, direction);

    return builder_texture_make_tex_obj(helper, basic, TEX_OBJ_PART, obj);
}

struct Center {
    struct IntPair length;
    struct IntPair index;
};

static struct ShiftTexPair** _append_children(struct Helper* helper, struct BasicTexObj* basic,
                                              enum TexPartDirection direction) {
    int shift = 0;
    int size = 0;
    int length = builder_texture_get_metric_by_direction(&helper->parent->basic, direction);
    int index = -1;

    struct ShiftTexPair** pairs =
        HELPER_ALLOCATE_ARRAY(helper, struct ShiftTexPair*, BUILDER_TEXTURE_MAX_ELEMENTS);

    struct RuleWithParent* rule = helper->rule;
    struct Obj** prop_objs = css_find_objs(rule->rule, "body");
    if (!prop_objs) {
        return NULL;
    }

    struct Obj* css_obj = NULL;
    css_iter (css_obj, prop_objs) {
        struct SelectorHelper child_helper = {
            .program = helper->program,
            .parent_rule = helper->rule,
            .selector = css_eval_rule(css_obj),
            .parent = helper->parent,
        };
        struct TexObj* child = builder_texture_build_tex_obj(&child_helper);
        if (!child) continue;
        int obj_length = builder_texture_get_metric_by_direction(&child->basic, direction);
        int end_shift = shift + obj_length;
        if (end_shift >= length) {
            break;
        }
        struct ShiftTexPair* pair = HELPER_ALLOCATE(helper, struct ShiftTexPair);
        builder_texture_resize_axis_by_direction(basic, end_shift, direction);
        builder_texture_resize_coaxis(basic, &child->basic, direction);
        builder_texture_fill_shift(pair, shift, direction);
        pair->obj = child;
        pairs[size++] = pair;
        shift = end_shift + builder_get_padding(helper->rule);
        index += 1;
    }

    pairs[size] = NULL;
    builder_texture_align(helper, pairs, basic, (struct IntPair){0, length},
                          (struct IntPair){0, index}, direction);
    builder_texture_justify(helper, pairs, basic, direction);

    return pairs;
}