#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/_draw_builder_texture_part.h"
#include "pixelopolis/_draw_builder_utils.h"
#include "pixelopolis/css_eval.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/draw_builder_texture.h"

static struct ShiftTexPair** _make_tiles(struct Helper* helper, int* grid_size);
static void _arange_tiles(struct Helper* helper, struct ShiftTexPair** tiles,
                          struct BasicTexObj* basic, int grid_size);
static struct ShiftTexPair** _drop_nulls(struct ShiftTexPair** tiles, int grid_size);

struct TexObj* builder_texture_build_tile(struct Helper* helper) {
    struct TexPartObj* obj = malloc(sizeof(struct TexPartObj));
    struct BasicTexObj basic = builder_texture_prepare_basic(helper);

    {
        struct SelectorHelper background_helper = MAKE_SELECTOR_HELPER(helper, "background");
        obj->background = builder_texture_build_tex_obj(&background_helper);
    }
    int grid_size;
    struct ShiftTexPair** tiles = _make_tiles(helper, &grid_size);
    _arange_tiles(helper, tiles, &basic, grid_size);
    obj->objs = _drop_nulls(tiles, grid_size);
    free(tiles);

    struct TexVoidObj* fill_obj = malloc(sizeof(struct TexVoidObj));
    fill_obj->child = builder_texture_make_tex_obj(helper, basic, TEX_OBJ_PART, obj);

    return builder_texture_make_tex_obj(helper, basic, TEX_OBJ_FILL, fill_obj);
}

struct Center {
    struct IntPair length;
    struct IntPair index;
};

static struct ShiftTexPair** _make_tiles(struct Helper* helper, int* grid_size) {
    int size = 0;
    *grid_size = 0;

    struct Rule* rule = helper->rule;
    struct Obj** prop_objs = css_find_objs(rule, "body");
    if (!prop_objs) {
        return NULL;
    }

    {
        struct Obj* css_obj = NULL;
        css_iter (css_obj, prop_objs) {
            *grid_size += 1;  // Count
        }
    }

    struct ShiftTexPair** pairs = malloc(sizeof(struct ShiftTexPair*) * *grid_size * *grid_size);
    struct SelectorHelper child_helper = {
        .program = helper->program,
        .parent_rule = helper->rule,
        .parent = helper->parent,
    };
    struct Obj* css_obj = NULL;
    size_t y_index = 0;
    css_iter (css_obj, prop_objs) {
        for (int x_index = 0; x_index < *grid_size; x_index++) {
            child_helper.selector = css_eval_rule(css_obj);
            struct TexObj* child = builder_texture_build_tex_obj(&child_helper);
            size_t index = ((y_index + x_index) % *grid_size) * *grid_size +
                           ((x_index + x_index) % *grid_size);
            if (child) {
                struct ShiftTexPair* pair = malloc(sizeof(struct ShiftTexPair));
                pair->obj = child;
                pairs[index] = pair;
            } else {
                pairs[index] = NULL;
            }
            size++;
        }
        y_index++;
    }

    pairs[size] = NULL;
    return pairs;
}

static void _arange_tiles(struct Helper* helper, struct ShiftTexPair** tiles,
                          struct BasicTexObj* basic, int grid_size) {
    int width = 0;
    int height = 0;
    int padding = builder_get_padding(helper->rule);

    // First, find max width and height.
    size_t size = grid_size * grid_size;
    for (size_t index = 0; index < grid_size; index++) {
        struct ShiftTexPair* tile = tiles[index];
        if (!tile) continue;
        width = width >= tile->obj->basic.width ? width : tile->obj->basic.width;
        height = height >= tile->obj->basic.height ? height : tile->obj->basic.height;
    }

    // Increase by padding
    width += padding;
    height += padding;

    // Shift tiles
    int shift[2] = {0, 0};
    for (size_t y_index = 0; y_index < grid_size; y_index++) {
        for (size_t x_index = 0; x_index < grid_size; x_index++) {
            size_t index = y_index * grid_size + x_index;
            struct ShiftTexPair* tile = tiles[index];
            if (!tile) continue;
            tile->shift[0] = x_index * width;
            tile->shift[1] = y_index * height;
        }
    }

    basic->width = width * grid_size;
    basic->height = height * grid_size;
}

static struct ShiftTexPair** _drop_nulls(struct ShiftTexPair** tiles, int grid_size) {
    size_t new_index = 0;
    struct ShiftTexPair** new_tiles =
        malloc(sizeof(struct ShiftTexPair*) * BUILDER_TEXTURE_MAX_ELEMENTS);
    size_t size = grid_size * grid_size;
    for (size_t index = 0; index < size; index++) {
        struct ShiftTexPair* tile = tiles[index];
        if (!tile) continue;
        new_tiles[new_index++] = tile;
    }
    new_tiles[new_index] = NULL;
    return new_tiles;
}