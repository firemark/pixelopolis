#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/_draw_builder_texture_part.h"
#include "pixelopolis/css/func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/draw_builder_texture.h"

static struct ShiftTexPair** _append_children(struct Helper* helper, struct BasicTexObj* basic,
                                              enum TexPartDirection direction);

struct TexObj* builder_texture_build_texture_part(struct Helper* helper,
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
    struct Center center = {
        .length = {.start = 0,
                   .end =
                       builder_texture_get_metric_by_direction(&helper->parent->basic, direction)},
        .index = {.start = 0, .end = -1},
    };

    struct ShiftTexPair** pairs =
        HELPER_ALLOCATE_ARRAY(helper, struct ShiftTexPair*, BUILDER_TEXTURE_MAX_ELEMENTS);

    struct SelectorHelper start_obj_helper = MAKE_SELECTOR_HELPER(helper, "start");
    struct TexObj* start_obj = builder_texture_build_tex_obj(&start_obj_helper);

    struct SelectorHelper end_obj_helper = MAKE_SELECTOR_HELPER(helper, "end");
    struct TexObj* end_obj = builder_texture_build_tex_obj(&end_obj_helper);

    if (start_obj) {
        int obj_length = builder_texture_get_metric_by_direction(&start_obj->basic, direction);
        if (obj_length >= center.length.end) {
            goto final;
        }
        struct ShiftTexPair* pair = HELPER_ALLOCATE(helper, struct ShiftTexPair);
        builder_texture_resize_axis_by_direction(basic, obj_length, direction);
        builder_texture_resize_coaxis(basic, &start_obj->basic, direction);
        builder_texture_fill_shift(pair, 0, direction);
        pair->obj = start_obj;
        pairs[size++] = pair;
        shift = obj_length + builder_get_padding(helper->rule);
        center.length.start = obj_length;
        center.index.start += 1;
        center.index.end += 1;
    }

    if (end_obj) {
        int obj_length = builder_texture_get_metric_by_direction(&end_obj->basic, direction);
        if (shift + obj_length >= center.length.end) {
            goto final;
        }
        center.length.end -= obj_length;
    }

    for (;;) {
        struct SelectorHelper center_helper = MAKE_SELECTOR_HELPER(helper, "center");
        struct TexObj* center_obj = builder_texture_build_tex_obj(&center_helper);
        if (!center_obj) {
            break;
        }
        int obj_length = builder_texture_get_metric_by_direction(&center_obj->basic, direction);
        int end_shift = shift + obj_length;
        if (end_shift >= center.length.end) {
            break;
        }
        if (center_obj) {
            struct ShiftTexPair* pair = HELPER_ALLOCATE(helper, struct ShiftTexPair);
            builder_texture_resize_axis_by_direction(basic, end_shift, direction);
            builder_texture_resize_coaxis(basic, &center_obj->basic, direction);
            builder_texture_fill_shift(pair, shift, direction);
            pair->obj = center_obj;
            pairs[size++] = pair;
        }
        shift = end_shift + builder_get_padding(helper->rule);
        center.index.end += 1;
    }

    if (end_obj) {
        int obj_length = builder_texture_get_metric_by_direction(&end_obj->basic, direction);
        int end_shift = shift + obj_length;
        struct ShiftTexPair* pair = HELPER_ALLOCATE(helper, struct ShiftTexPair);
        builder_texture_resize_axis_by_direction(basic, end_shift, direction);
        builder_texture_resize_coaxis(basic, &end_obj->basic, direction);
        builder_texture_fill_shift(pair, shift, direction);
        pair->obj = end_obj;
        pairs[size++] = pair;
    }

final:
    pairs[size] = NULL;
    builder_texture_align(helper, pairs, basic, center.length, center.index, direction);
    builder_texture_justify(helper, pairs, basic, direction);
    return pairs;
}
