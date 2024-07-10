#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/draw_builder_texture.h"

static void _append_children(struct Helper* helper, struct TexPartObj* obj,
                             struct BasicTexObj* basic, enum TexPartDirection direction);
static void _align(struct Helper* helper, struct TexPartObj* obj, struct BasicTexObj* basic,
                   int start, int end, int start_index, int end_index,
                   enum TexPartDirection direction);
static void _coalign(struct Helper* helper, struct TexPartObj* obj, struct BasicTexObj* basic,
                   int start, int end, int start_index, int end_index,
                   enum TexPartDirection direction);

static int _get_pair_axis(enum TexPartDirection direction);
static int _get_metric(struct BasicTexObj* basic, enum TexPartDirection direction);
static void _resize_axis(struct BasicTexObj* a, int len, enum TexPartDirection direction);
static void _resize_coaxis(struct BasicTexObj* a, struct BasicTexObj* b,
                           enum TexPartDirection direction);
static void _fill_shift(struct ShiftTexPair* pair, int shift, enum TexPartDirection direction);

struct TexObj* builder_texture_build_texture_part(struct Helper* helper,
                                                  enum TexPartDirection direction) {
    struct BasicTexObj basic = {.width = 0, .height = 0};
    struct TexPartObj* obj = malloc(sizeof(struct TexPartObj));
    obj->padding = builder_get_padding(helper->rule);

    {
        struct SelectorHelper background_helper = MAKE_SELECTOR_HELPER(helper, "background");
        obj->background = builder_texture_build_tex_obj(&background_helper);
    }
    _append_children(helper, obj, &basic, direction);

    return builder_texture_make_tex_obj(helper, basic, TEX_OBJ_PART, obj);
}

static void _append_children(struct Helper* helper, struct TexPartObj* obj,
                             struct BasicTexObj* basic, enum TexPartDirection direction) {
    int length = _get_metric(&helper->parent->basic, direction);
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
        int obj_length = _get_metric(&start_obj->basic, direction);
        if (obj_length >= length) {
            goto final;
        }
        struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
        _resize_axis(basic, obj_length, direction);
        _resize_coaxis(basic, &start_obj->basic, direction);
        _fill_shift(pair, 0, direction);
        pair->obj = start_obj;
        obj->objs[size++] = pair;
        shift = obj_length + obj->padding;
        start = obj_length;
        start_center_index += 1;
        end_center_index += 1;
    }

    if (end_obj) {
        int obj_length = _get_metric(&end_obj->basic, direction);
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
        int obj_length = _get_metric(&center_obj->basic, direction);
        int end_shift = shift + obj_length;
        if (end_shift >= length) {
            break;
        }
        if (center_obj) {
            struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
            _resize_axis(basic, end_shift, direction);
            _resize_coaxis(basic, &center_obj->basic, direction);
            _fill_shift(pair, shift, direction);
            pair->obj = center_obj;
            obj->objs[size++] = pair;
        }
        shift = end_shift + obj->padding;
        end_center_index += 1;
    }

    if (end_obj) {
        int obj_length = _get_metric(&end_obj->basic, direction);
        int end_shift = shift + obj_length;
        struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
        _resize_axis(basic, end_shift, direction);
        _resize_coaxis(basic, &end_obj->basic, direction);
        _fill_shift(pair, shift, direction);
        pair->obj = end_obj;
        obj->objs[size++] = pair;
    }

final:
    obj->objs[size] = NULL;
    obj->objs_size = size;
    _align(helper, obj, basic, start, length, start_center_index, end_center_index, direction);
}

static inline void _add_margin_to_tex_objs(struct TexPartObj* obj, int index, int end_index,
                                           int margin, enum TexPartDirection direction) {
    for (; index <= end_index; index++) {
        struct ShiftTexPair* pair = obj->objs[index];
        pair->shift[_get_pair_axis(direction)] += margin;
    }
}

static inline int _get_margin_to_align(struct ShiftTexPair* pair, int end,
                                       enum TexPartDirection direction) {
    int shift = pair->shift[_get_pair_axis(direction)];
    int length = _get_metric(&pair->obj->basic, direction);
    return end - shift - length;
}

#define IF_NAME(name) else if (!strcmp(align, name))
static void _align(struct Helper* helper, struct TexPartObj* obj, struct BasicTexObj* basic,
                   int start, int end, int start_index, int end_index,
                   enum TexPartDirection direction) {
    if (start_index > end_index) {
        return;
    }
    char* align = css_find_selector_element_prop(helper->rule, "align");
    struct ShiftTexPair* last_pair = obj->objs[end_index];

    if (!align) return;
    IF_NAME("start") {
        // default is start aligned
        // so we have done a job
    }
    IF_NAME("end") {
        // default is start aligned so we need to
        // find distance between last texture and end of wall
        // and add to shifts
        int margin = _get_margin_to_align(last_pair, end - start, direction);
        _add_margin_to_tex_objs(obj, start_index, end_index, margin, direction);
        _resize_axis(basic, end, direction);
    }
    IF_NAME("center") {
        // default is start aligned so we need to
        // find distance between last texture and end of wall
        // div by 2
        // and add to shifts
        int margin = _get_margin_to_align(last_pair, end - start, direction) / 2;
        _add_margin_to_tex_objs(obj, start_index, end_index, margin, direction);
        _resize_axis(basic, end, direction);
    }
    IF_NAME("justify") {
        // we need to find a last margin and rescale other margins
        // and shift last element to the end
        int end_margin = _get_margin_to_align(last_pair, end - start, direction);

        // find a scale of last margin
        int shift = last_pair->shift[_get_pair_axis(direction)];
        float ratio = (float)(shift + end_margin) / (float)shift;

        // rescale margins (except last item)
        size_t index;
        for (index = start_index; index < end_index; index++) {
            struct ShiftTexPair* next_pair = obj->objs[index + 1];
            // move next element to increase margin
            next_pair->shift[_get_pair_axis(direction)] *= ratio;
        }
        _resize_axis(basic, end, direction);
    }
}
#undef IF_NAME

static int _get_metric(struct BasicTexObj* basic, enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            return basic->width;
        case TEX_PART_HORIZONTAL:
            return basic->height;
        default:
            return 0;
    }
}

static int _get_pair_axis(enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            return 0;
        case TEX_PART_HORIZONTAL:
            return 1;
        default:
            return 0;
    }
}

static void _resize_axis(struct BasicTexObj* a, int len, enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            a->width = a->width >= len ? a->width : len;
            break;
        case TEX_PART_HORIZONTAL:
            a->height = a->height >= len ? a->height : len;
            break;
    }
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