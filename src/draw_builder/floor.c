#include <stdlib.h>
#include "css_func.h"
#include "_draw_builder.h"

struct ShiftTexPair* _make_shift_pair(int shift, struct TexObj* obj) {
    struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
    pair->shift = shift;
    pair->obj = obj;
    return pair;
}

int _get_tex_padding(struct Rule* rule) {
    int* padding_ptr = css_find_number_prop(rule, "padding");
    return padding_ptr ? *padding_ptr : 0;
}

int _get_tex_padding_with_name(struct Rule* rule, char* name) {
    int* padding_ptr = css_find_number_prop(rule, name);
    return padding_ptr ? *padding_ptr : _get_tex_padding(rule);
}

struct ShiftTexPair* _make_left_pair(struct Helper* helper, int *end_width, int* shift) {
    struct SelectorHelper left_helper = {
        .program=helper->program,
        .parent_rule=helper->rule,
        .selector=css_find_selector_prop(helper->rule, "left"),
        .parent=helper->parent,
    };
    struct TexObj* left = builder_build_texture(&left_helper);
    if (!left) return NULL;
    if (!left->texture) return NULL;

    int left_width = left->texture->width;
    if (left_width >= *end_width) return NULL;

    struct ShiftTexPair* pair = _make_shift_pair(*shift, left);
    int padding = _get_tex_padding_with_name(helper->rule, "left-padding");
    *shift += left_width + padding;
    return pair;
}

struct ShiftTexPair* _make_right_pair(struct Helper* helper, int *end_width, int* shift) {
    struct SelectorHelper right_helper = {
        .program=helper->program,
        .parent_rule=helper->rule,
        .selector=css_find_selector_prop(helper->rule, "right"),
        .parent=helper->parent,
    };
    struct TexObj* right = builder_build_texture(&right_helper);
    if (!right) return NULL;
    if (!right->texture) return NULL;

    int right_width = right->texture->width;
    if (*shift + right_width >= *end_width) return NULL;

    struct ShiftTexPair* pair = _make_shift_pair(*shift, right);
    *end_width -= right_width;
    return pair;
}

void _append_objs_to_floor(struct Helper* helper, struct FloorObj* floor, int end_width) {
    int shift = 0;
    int size = 0;
    struct ShiftTexPair** objs = malloc(sizeof(struct ShiftTexPair*) * BUILDER_MAX_ELEMENTS);
    struct Rule* rule = helper->rule;
    struct Program* program = helper->program;

    struct ShiftTexPair* left_pair = _make_left_pair(helper, &end_width, &shift);
    struct ShiftTexPair* right_pair = _make_right_pair(helper, &end_width, &shift);

    int right_padding =  _get_tex_padding_with_name(rule, "right-padding");

    if (left_pair) { // add left as first
        objs[size++] = left_pair;
    }

    for(;;) {
        struct SelectorHelper middle_helper = {
            .program=program,
            .parent_rule=rule,
            .selector=css_find_selector_prop(rule, "middle"),
            .parent=helper->parent,
        };
        struct TexObj* middle = builder_build_texture(&middle_helper);
        int middle_width = middle && middle->texture ? middle->texture->width : 12;
        if (shift + middle_width + right_padding >= end_width) break; // TODO - remove created tex-obj and to avoid memory leak
        objs[size++] = _make_shift_pair(shift, middle);
        shift += middle_width + _get_tex_padding_with_name(rule, "middle-padding");
    }

    if (right_pair) { // add right as last
        objs[size++] = right_pair;
    }

    // final
    objs[size] = NULL;
    floor->objs = objs;
    floor->objs_size = size;
}

int _get_floor_height(int *height_ptr, struct FloorObj* obj) {
    if (height_ptr) return *height_ptr;
    if (obj->tex && obj->tex->texture) return obj->tex->texture->height;
    return 0;
}

struct FloorObj* builder_build_floor(struct SelectorHelper* helper, int wall_width) {
    struct Rule *rule = builder_make_rule_from_helper(helper);
    if (!rule) return NULL;
    int* height_ptr = css_find_number_prop(rule, "height");

    struct FloorObj* floor = malloc(sizeof(struct FloorObj));
    floor->height = _get_floor_height(height_ptr, floor);

    struct SelectorHelper tex_helper = {
        .program=helper->program,
        .parent_rule=rule,
        .selector=css_find_selector_prop(rule, "texture"),
        .parent=helper->parent,
    };
    floor->tex = builder_build_texture(&tex_helper);

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=rule,
        .parent=helper->parent,
    };
    _append_objs_to_floor(&inner_helper, floor, wall_width);

    css_free_rule_half(rule);
    return floor;
}
