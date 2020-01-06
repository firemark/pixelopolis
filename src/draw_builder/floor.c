#include <stdlib.h>
#include "css_func.h"
#include "_draw_builder.h"

void _append_objs_to_floor(struct Helper* helper, struct FloorObj* floor, int wall_width) {
    int x = 0;
    int size = 0;
    int padding = floor->padding;
    struct TexObj** objs = malloc(sizeof(struct TexObj*) * BUILDER_MAX_ELEMENTS);
    struct Rule* rule = helper->rule;
    struct Program* program = helper->program;

    struct TexObj* left = NULL; 
    struct TexObj* right = NULL;

    struct SelectorHelper left_helper = {
        .program=program,
        .parent_rule=rule,
        .selector=css_find_selector_prop(rule, "left"),
        .parent=helper->parent,
    };
    left = builder_build_texture(&left_helper);
    if (left && left->texture) {
        if (left->texture->width < wall_width) {
            x += left->texture->width + padding;
        } else {
            left = NULL;
            right = NULL;
            goto final;
        }
    }

    struct SelectorHelper right_helper = {
        .program=program,
        .parent_rule=rule,
        .selector=css_find_selector_prop(rule, "right"),
        .parent=helper->parent,
    };
    right = builder_build_texture(&right_helper);
    if (right && right->texture) {
        if (x + right->texture->width < wall_width) {
            wall_width -= right->texture->width;
        } else {
            right = NULL;
            goto final;
        }
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
        if (x + middle_width >= wall_width) break;
        x += middle_width + padding;
        objs[size++] = middle;
    }

final:
    objs[size] = NULL;
    floor->objs = objs;
    floor->objs_size = size;
    floor->right = right;
    floor->left = left;
}

int _get_floor_height(int *height_ptr, struct FloorObj* obj) {
    if (height_ptr) {
        return *height_ptr;
    }  
    if (obj->tex && obj->tex->texture) {
        return obj->tex->texture->height;
    } 

    return 0;
}

struct FloorObj* builder_build_floor(struct SelectorHelper* helper, int wall_width) {
    struct Rule *rule = builder_make_rule_from_helper(helper);
    if (!rule) return NULL;
    int* height_ptr = css_find_number_prop(rule, "height");
    int* padding_ptr = css_find_number_prop(rule, "padding");

    struct FloorObj* floor = malloc(sizeof(struct FloorObj));
    floor->padding = padding_ptr ? *padding_ptr : 0;
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
