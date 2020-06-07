#include <stdlib.h>
#include <string.h>
#include "css_func.h"
#include "_draw_builder.h"

#define DEFAULT_WIDTH 12

struct ShiftTexPair* _make_shift_pair(int shift, struct TexObj* obj) {
    struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
    pair->shift = shift;
    pair->obj = obj;
    return pair;
}

int _get_tex_padding_with_name(struct Rule* rule, char* name) {
    int* padding_ptr = css_find_number_prop(rule, name);
    return padding_ptr ? *padding_ptr : builder_get_padding(rule);
}

int _get_tex_obj_width(struct TexObj* obj) {
    return obj && obj->texture ? obj->texture->width : DEFAULT_WIDTH;
}

struct TexObj* _make_tex_obj(struct Helper* helper, char* name) {
    struct SelectorHelper tex_helper = {
        .program=helper->program,
        .parent_rule=helper->rule,
        .selector=css_find_selector_prop(helper->rule, name),
        .parent=helper->parent,
    };
    struct TexObj* tex = builder_build_texture(&tex_helper);
    if (!tex) return NULL;
    if (!tex->texture) return NULL;
    return tex;
}

struct ShiftTexPair* _make_left_pair(struct Helper* helper, int *end_width, int* shift) {
    struct ShiftTexPair* pair = NULL;
    struct TexObj* left = _make_tex_obj(helper, "left");
    if (!left) goto final;

    int left_width = left->texture->width;
    if (left_width > *end_width) goto final;

    pair = _make_shift_pair(*shift, left);
    *shift += left_width;
final:
    *shift += _get_tex_padding_with_name(helper->rule, "left-padding");
    return pair;
}

struct ShiftTexPair* _make_right_pair(struct Helper* helper, int *end_width, int* shift) {
    struct ShiftTexPair* pair = NULL;
    struct TexObj* right = _make_tex_obj(helper, "right");
    if (!right) goto final;

    int right_width = right->texture->width;
    if (*shift + right_width > *end_width) goto final;

    *end_width -= right_width;
    pair = _make_shift_pair(*end_width, right);
final:
    *end_width -= _get_tex_padding_with_name(helper->rule, "right-padding");
    return pair;
}


void _add_margin_to_tex_objs(struct ShiftTexPair **pairs, size_t start_index, size_t end_index, int margin) {
    size_t index;
    for(index = start_index; index <= end_index; index++) {
        struct ShiftTexPair* pair = pairs[index];
        pair->shift += margin;
    }
}

int _get_margin_to_align_floor(struct ShiftTexPair *pair, int end_width) {
    int margin = pair->shift + _get_tex_obj_width(pair->obj);
    return end_width - margin;
}

#define IF_NAME(name) else if (!strcmp(align, name))
void _align_tex_objs(
        struct Helper* helper,
        struct ShiftTexPair **pairs, size_t size,
        int start_width, int end_width,
        char with_left, char with_right) {
    if (size == 0) {
        return;
    }
    char* align = css_find_selector_element_prop(helper->rule, "align");
    size_t start_index = with_left ? 1 : 0;
    size_t end_index = with_right ? size - 2 : size - 1;
    struct ShiftTexPair* last_pair = pairs[end_index];

    if (!align) return; // default is left
    IF_NAME("left") {
        // default is left aligned
        // so we have done a job
    }
    IF_NAME("right") {
        // default is left aligned so we need to
        // find distance between last texture and end of wall
        // and add to shifts
        int margin = _get_margin_to_align_floor(last_pair, end_width);
        _add_margin_to_tex_objs(pairs, start_index, end_index, margin);
    }
    IF_NAME("middle") {
        // default is left aligned so we need to
        // find distance between last texture and end of wall
        // div by 2
        // and add to shifts
        int margin = _get_margin_to_align_floor(last_pair, end_width) / 2;
        _add_margin_to_tex_objs(pairs, start_index, end_index, margin);
    }
    IF_NAME("justify") {
        // we need to find a last margin and rescale other margins
        // and shift last element to the end
        int right_margin = _get_margin_to_align_floor(last_pair, end_width);

        // find a scale of last margin
        float ratio = (float)(last_pair->shift + right_margin) / (float)last_pair->shift;

        // rescale margins (except last item)
        size_t index;
        for(index = start_index; index < end_index; index++) {
            struct ShiftTexPair* next_pair = pairs[index + 1];
            // move next element to increase margin
            next_pair->shift *= ratio;
        }
    }
}
#undef IF_NAME

void _append_objs_to_floor(struct Helper* helper, struct FloorObj* floor, int end_width) {
    int shift = 0;
    int size = 0;
    int start_width = 0;
    struct ShiftTexPair** objs = malloc(sizeof(struct ShiftTexPair*) * BUILDER_MAX_ELEMENTS);
    struct Rule* rule = helper->rule;

    struct ShiftTexPair* left_pair = _make_left_pair(helper, &end_width, &shift);
    struct ShiftTexPair* right_pair = _make_right_pair(helper, &end_width, &shift);

    if (left_pair) { // add left as first
        objs[size++] = left_pair;
        start_width = shift;
    }

    for(;;) {
        struct TexObj* middle = _make_tex_obj(helper, "middle");
        int middle_width = _get_tex_obj_width(middle);
        if (shift + middle_width > end_width) break; // TODO - remove created tex-obj and to avoid memory leak
        objs[size++] = _make_shift_pair(shift, middle);
        shift += middle_width + _get_tex_padding_with_name(rule, "middle-padding");
    }

    if (right_pair) { // add right as last
        objs[size++] = right_pair;
    }

    _align_tex_objs(helper, objs, size, start_width, end_width, !!left_pair, !!right_pair);

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

    floor->height = _get_floor_height(height_ptr, floor);

    css_free_rule_half(rule);
    return floor;
}
