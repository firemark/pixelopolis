#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/draw_builder_texture.h"

#define SET_GREATER(attr) a->attr = a->attr >= b->attr ? a->attr : b->attr

static void _append_children(struct Helper* helper, struct TexPartObj* obj,
                             struct BasicTexObj* basic, enum TexPartDirection direction);
static int _get_metric(struct BasicTexObj* basic, enum TexPartDirection direction);
static void _add_max(struct BasicTexObj* a, struct BasicTexObj* b, enum TexPartDirection direction);

struct TexObj* builder_texture_build_texture_part(struct Helper* helper,
                                                  enum TexPartDirection direction) {
    struct BasicTexObj basic;
    struct TexPartObj* obj = malloc(sizeof(struct TexPartObj));
    obj->direction = direction;
    obj->padding = builder_get_padding(helper->rule);

    {
        struct SelectorHelper background_helper = MAKE_SELECTOR_HELPER(helper, "background");
        obj->background = builder_texture_build_tex_obj(&background_helper);
    }
    _append_children(helper, obj, &basic, direction);

    return builder_texture_make_tex_obj(helper, basic, TEX_OBJ_SECTION, obj);
}

static void _append_children(struct Helper* helper, struct TexPartObj* obj,
                             struct BasicTexObj* basic, enum TexPartDirection direction) {
    int length = _get_metric(&helper->parent->basic, direction);
    int shift = 0;
    int size = 0;

    obj->objs = malloc(sizeof(struct TexObj*) * BUILDER_TEXTURE_MAX_ELEMENTS);

    {
        struct SelectorHelper begin_obj_helper = MAKE_SELECTOR_HELPER(helper, "begin");
        struct TexObj* begin_obj = builder_texture_build_tex_obj(&begin_obj_helper);
        if (begin_obj) {
            int obj_length = _get_metric(&begin_obj->basic, direction);
            if (obj_length < length) {
                _add_max(basic, &begin_obj->basic, direction);
                struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
                pair->shift = shift;
                pair->obj = begin_obj;
                obj->objs[size++] = pair;
                shift += obj_length + obj->padding;
            } else {
                goto final;
            }
        }
    }

    {
        struct SelectorHelper end_obj_helper = MAKE_SELECTOR_HELPER(helper, "end");
        struct TexObj* end_obj = builder_texture_build_tex_obj(&end_obj_helper);
        if (end_obj) {
            int obj_length = _get_metric(&end_obj->basic, direction);
            if (shift + obj_length < length) {
                _add_max(basic, &end_obj->basic, direction);
                struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
                pair->shift = shift;
                pair->obj = end_obj;
                obj->objs[size++] = pair;
                length -= obj_length;
            } else {
                goto final;
            }
        }
    }

    for (;;) {
        struct SelectorHelper center_helper = MAKE_SELECTOR_HELPER(helper, "center");
        struct TexObj* center_obj = builder_texture_build_tex_obj(&center_helper);
        int obj_length = center_obj ? _get_metric(&center_obj->basic, direction) : 12;
        if (shift + obj_length >= length) {
            break;
        }

        _add_max(basic, &center_obj->basic, direction);
        struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
        pair->shift = shift;
        pair->obj = center_obj;
        obj->objs[size++] = pair;
        shift += obj_length + obj->padding;
    }

final:
    obj->objs[size] = NULL;
    obj->objs_size = size;
}

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

static void _add_max(struct BasicTexObj* a, struct BasicTexObj* b,
                     enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            a->width += b->width;
            SET_GREATER(height);
            break;
        case TEX_PART_HORIZONTAL:
            a->height += b->height;
            SET_GREATER(width);
            break;
    }
}
