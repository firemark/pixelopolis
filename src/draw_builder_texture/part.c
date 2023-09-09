#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css_func.h"

static void _append_children(struct Helper* helper, struct TexPartObj* obj, int width, int height);

struct TexObj* builder_texture_build_texture_part(struct Helper* helper, enum TexObjType type,
                                                  enum TexPartDirection direction) {
    struct Rule* rule = helper->rule;
    struct TexPartObj* obj = malloc(sizeof(struct TexPartObj));
    obj->direction = direction;
    obj->padding = builder_get_padding(rule);

    {
        struct SelectorHelper background_helper = {
            .program = helper->program,
            .parent_rule = rule,
            .selector = css_find_selector_prop(rule, "background"),
            .parent = helper->parent,
        };
        obj->background = builder_texture_build_tex_obj(&background_helper);
    }
    {
        struct Helper inner_helper = {
            .program = helper->program,
            .rule = rule,
            .parent = helper->parent,
        };
        _append_children(&inner_helper, obj, 0, 0);
    }

    struct TexObj* tex_obj = malloc(sizeof(struct TexObj));
    tex_obj->type = type;
    tex_obj->obj = obj;

    return tex_obj;
}

static void _append_children(struct Helper* helper, struct TexPartObj* obj, int length) {
    int shift = 0;
    int size = 0;

    obj->objs = malloc(sizeof(struct TexObj*) * BUILDER_TEXTURE_MAX_ELEMENTS);
    struct Rule* rule = helper->rule;
    struct Program* program = helper->program;

    {
        struct SelectorHelper begin_obj_helper = {
            .program = program,
            .parent_rule = rule,
            .selector = css_find_selector_prop(rule, "begin"),
            .parent = helper->parent,
        };
        struct TexObj* begin_obj = builder_texture_build_tex_obj(&begin_obj_helper);
        if (begin_obj) {
            if (begin_obj->length < length) {
                struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
                pair->shift = shift;
                pair->obj = begin_obj;
                obj->objs[size++] = pair;
                shift += begin_obj->length + obj->padding;
            } else {
                goto final;
            }
        }
    }

    {
        struct SelectorHelper end_obj_helper = {
            .program = program,
            .parent_rule = rule,
            .selector = css_find_selector_prop(rule, "end"),
            .parent = helper->parent,
        };
        struct TexObj* end_obj = builder_texture_build_tex_obj(&end_obj_helper);
        if (end_obj) {
            if (shift + end_obj->length < length) {
                struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
                pair->shift = shift;
                pair->obj = end_obj;
                obj->objs[size++] = pair;
                length -= end_obj->length; 
            } else {
                goto final;
            }
        }
    }

    for (;;) {
        struct SelectorHelper center_helper = {
            .program = program,
            .parent_rule = rule,
            .selector = css_find_selector_prop(rule, "center"),
            .parent = helper->parent,
        };
        struct TexObj* center_obj = builder_texture_build_tex_obj(&center_helper);
        int center_length = center_obj ? center_length : 12;
        if (shift + center_length >= length) { break; }

        struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
        pair->shift = shift;
        pair->obj = center_obj;
        obj->objs[size++] = pair;
        shift += center_length + obj->padding;
    }

final:
    obj->objs[size] = NULL;
    obj->objs_size = size;
}