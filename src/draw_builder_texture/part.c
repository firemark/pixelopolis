#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/draw_builder_texture.h"

static void _append_children(struct Helper* helper, struct TexPartObj* obj,
                             struct BasicTexObj* basic, enum TexPartDirection direction);
static void _coalign(struct Helper* helper, struct TexPartObj* obj, struct BasicTexObj* basic,
                     int start, int end, int start_index, int end_index,
                     enum TexPartDirection direction);

static void _resize_coaxis(struct BasicTexObj* a, struct BasicTexObj* b,
                           enum TexPartDirection direction);
static void _fill_shift(struct ShiftTexPair* pair, int shift, enum TexPartDirection direction);

struct TexObj* builder_texture_build_texture_part(struct Helper* helper,
                                                  enum TexPartDirection direction) {
    struct TexPartObj* obj = malloc(sizeof(struct TexPartObj));
    struct BasicTexObj basic = builder_texture_prepare_basic(helper);

    {
        struct SelectorHelper background_helper = MAKE_SELECTOR_HELPER(helper, "background");
        obj->background = builder_texture_build_tex_obj(&background_helper);
    }
    _append_children(helper, obj, &basic, direction);

    return builder_texture_make_tex_obj(helper, basic, TEX_OBJ_PART, obj);
}

static void _append_children(struct Helper* helper, struct TexPartObj* obj,
                             struct BasicTexObj* basic, enum TexPartDirection direction) {
    int length = builder_texture_get_metric_by_direction(&helper->parent->basic, direction);
    int shift = 0;
    int size = 0;
    int start_center_index = 0;
    int end_center_index = -1;
    int start = 0;

    obj->objs = malloc(sizeof(struct TexObj*) * BUILDER_TEXTURE_MAX_ELEMENTS);

    struct SelectorHelper start_obj_helper = MAKE_SELECTOR_HELPER(helper, "start");
    struct TexObj* start_obj = builder_texture_build_tex_obj(&start_obj_helper);

    struct SelectorHelper end_obj_helper = MAKE_SELECTOR_HELPER(helper, "end");
    struct TexObj* end_obj = builder_texture_build_tex_obj(&end_obj_helper);

    if (start_obj) {
        int obj_length = builder_texture_get_metric_by_direction(&start_obj->basic, direction);
        if (obj_length >= length) {
            goto final;
        }
        struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
        builder_texture_resize_axis_by_direction(basic, obj_length, direction);
        _resize_coaxis(basic, &start_obj->basic, direction);
        _fill_shift(pair, 0, direction);
        pair->obj = start_obj;
        obj->objs[size++] = pair;
        shift = obj_length + builder_get_padding(helper->rule);
        start = obj_length;
        start_center_index += 1;
        end_center_index += 1;
    }

    if (end_obj) {
        int obj_length = builder_texture_get_metric_by_direction(&end_obj->basic, direction);
        if (shift + obj_length >= length) {
            goto final;
        }
        length -= obj_length;
    }

    for (;;) {
        struct SelectorHelper center_helper = MAKE_SELECTOR_HELPER(helper, "center");
        struct TexObj* center_obj = builder_texture_build_tex_obj(&center_helper);
        if (!center_obj) {
            break;
        }
        int obj_length = builder_texture_get_metric_by_direction(&center_obj->basic, direction);
        int end_shift = shift + obj_length;
        if (end_shift >= length) {
            break;
        }
        if (center_obj) {
            struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
            builder_texture_resize_axis_by_direction(basic, end_shift, direction);
            _resize_coaxis(basic, &center_obj->basic, direction);
            _fill_shift(pair, shift, direction);
            pair->obj = center_obj;
            obj->objs[size++] = pair;
        }
        shift = end_shift + builder_get_padding(helper->rule);
        end_center_index += 1;
    }

    if (end_obj) {
        int obj_length = builder_texture_get_metric_by_direction(&end_obj->basic, direction);
        int end_shift = shift + obj_length;
        struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
        builder_texture_resize_axis_by_direction(basic, end_shift, direction);
        _resize_coaxis(basic, &end_obj->basic, direction);
        _fill_shift(pair, shift, direction);
        pair->obj = end_obj;
        obj->objs[size++] = pair;
    }

final:
    obj->objs[size] = NULL;
    builder_texture_align(helper, obj, basic, start, length, start_center_index, end_center_index,
                          direction);
}

#define SET_GREATER(attr) a->attr = a->attr >= b->attr ? a->attr : b->attr
static void _resize_coaxis(struct BasicTexObj* a, struct BasicTexObj* b,
                           enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            SET_GREATER(height);
            break;
        case TEX_PART_HORIZONTAL:
            SET_GREATER(width);
            break;
    }
}
#undef SET_GREATER

static void _fill_shift(struct ShiftTexPair* pair, int shift, enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            pair->shift[0] = shift;
            pair->shift[1] = 0;
            break;
        case TEX_PART_HORIZONTAL:
            pair->shift[0] = 0;
            pair->shift[1] = shift;
            break;
    }
}