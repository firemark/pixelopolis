#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/_draw_builder_texture_part.h"
#include "pixelopolis/_draw_builder_utils.h"
#include "pixelopolis/css/eval.h"
#include "pixelopolis/css/func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/draw_builder_texture.h"

static struct ShiftTexPair** _make_tiles(struct Helper* helper, int* grid_size);
static struct ShiftTexPair** _arange_tiles(struct Helper* helper, struct ShiftTexPair** tiles,
                                           struct BasicTexObj* basic, int grid_size);
static struct ShiftTexPair** _drop_nulls(struct ShiftTexPair** tiles, int grid_size);
static struct ShiftTexPair** _concat(struct Helper* helper, struct ShiftTexPair** a,
                                     struct ShiftTexPair** b);

struct TexObj* builder_texture_build_tile(struct Helper* helper) {
    struct TexPartObj* obj = HELPER_ALLOCATE(helper, struct TexPartObj);
    struct BasicTexObj basic = builder_texture_prepare_basic(helper);

    {
        struct SelectorHelper background_helper = MAKE_SELECTOR_HELPER(helper, "background");
        obj->background = builder_texture_build_tex_obj(&background_helper);
    }
    int grid_size = 0;
    struct ShiftTexPair** tiles = _make_tiles(helper, &grid_size);
    struct ShiftTexPair** additional_tiles = _arange_tiles(helper, tiles, &basic, grid_size);

    tiles = _drop_nulls(tiles, grid_size);
    additional_tiles = _drop_nulls(additional_tiles, grid_size);
    obj->objs = _concat(helper, tiles, additional_tiles);

    struct TexVoidObj* fill_obj = HELPER_ALLOCATE(helper, struct TexVoidObj);
    fill_obj->child = builder_texture_make_tex_obj(helper, basic, TEX_OBJ_PART, obj);

    return builder_texture_make_tex_obj(helper, basic, TEX_OBJ_FILL, fill_obj);
}

struct Center {
    struct IntPair length;
    struct IntPair index;
};

static struct ShiftTexPair** _make_tiles(struct Helper* helper, int* grid_size_out) {
    int non_repeat_grid_size = 0;

    struct RuleWithParent* rule = helper->rule;
    struct Obj** prop_objs = css_find_objs(rule->rule, "body");
    if (!prop_objs) {
        return NULL;
    }

    {
        struct Obj* css_obj = NULL;
        css_iter (css_obj, prop_objs) {
            non_repeat_grid_size += 1;  // Count
        }
    }

    int repeat = builder_get_int(rule, "repeat", 1);
    int grid_size = non_repeat_grid_size * repeat;
    *grid_size_out = grid_size;

    struct ShiftTexPair** pairs = malloc(sizeof(struct ShiftTexPair*) * (grid_size * grid_size));
    struct SelectorHelper child_helper = {
        .program = helper->program,
        .parent_rule = helper->rule,
        .parent = helper->parent,
    };

    for (int repeat_index = 0; repeat_index < repeat; repeat_index++) {
        struct Obj* css_obj = NULL;
        int y_index = repeat_index * non_repeat_grid_size;
        css_iter (css_obj, prop_objs) {
            for (int x_index = 0; x_index < grid_size; x_index++) {
                child_helper.selector = css_eval_rule(css_obj);
                struct TexObj* child = builder_texture_build_tex_obj(&child_helper);
                size_t index =
                    ((y_index + x_index) % grid_size) * grid_size + (x_index % grid_size);
                if (child) {
                    struct ShiftTexPair* pair = HELPER_ALLOCATE(helper, struct ShiftTexPair);
                    pair->obj = child;
                    pairs[index] = pair;
                } else {
                    pairs[index] = NULL;
                }
            }
            y_index++;
        }
    }

    return pairs;
}

static struct ShiftTexPair** _arange_tiles(struct Helper* helper, struct ShiftTexPair** tiles,
                                           struct BasicTexObj* basic, int grid_size) {
    int width = 0;
    int height = 0;
    int padding = builder_get_padding(helper->rule);
    int x_shift = builder_get_int(helper->rule, "shift", 0);
    // TODO: support y_shift

    int size = grid_size * grid_size;

    // Add array with additional tiles
    struct ShiftTexPair** additional_tiles = malloc(sizeof(struct ShiftTexPair*) * size);
    for (int index = 0; index < size; index++) {
        additional_tiles[index] = NULL;
    }

    // First, find max width and height.
    for (int index = 0; index < grid_size; index++) {
        struct ShiftTexPair* tile = tiles[index];
        if (!tile) continue;
        width = width >= tile->obj->basic.width ? width : tile->obj->basic.width;
        height = height >= tile->obj->basic.height ? height : tile->obj->basic.height;
    }

    // Increase by padding
    width += padding;
    height += padding;

    // Set size of tileset
    basic->width = width * grid_size;
    basic->height = height * grid_size;

    // Shift tiles
    for (int y_index = 0; y_index < grid_size; y_index++) {
        for (int x_index = 0; x_index < grid_size; x_index++) {
            size_t index = y_index * grid_size + x_index;
            struct ShiftTexPair* tile = tiles[index];
            if (!tile) continue;
            tile->shift[0] = x_index * width + y_index * x_shift;
            tile->shift[1] = y_index * height;
            if (tile->shift[0] + tile->obj->basic.width > basic->width) {
                // Warping
                struct ShiftTexPair* additional_tile = HELPER_ALLOCATE(helper, struct ShiftTexPair);
                additional_tile->obj = tile->obj;
                additional_tile->shift[0] = tile->shift[0] - basic->width;
                additional_tile->shift[1] = tile->shift[1];
                additional_tiles[index] = additional_tile;
            }
        }
    }

    return additional_tiles;
}

static struct ShiftTexPair** _drop_nulls(struct ShiftTexPair** tiles, int grid_size) {
    size_t new_index = 0;
    int size = grid_size * grid_size;
    struct ShiftTexPair** new_tiles = malloc(sizeof(struct ShiftTexPair*) * (size + 1));

    for (int index = 0; index < size; index++) {
        struct ShiftTexPair* tile = tiles[index];
        if (!tile) continue;
        new_tiles[new_index++] = tile;
    }
    new_tiles[new_index] = NULL;
    free(tiles);
    return new_tiles;
}

static struct ShiftTexPair** _concat(struct Helper* helper, struct ShiftTexPair** a,
                                     struct ShiftTexPair** b) {
    size_t size_a = 0;
    size_t size_b = 0;
    struct ShiftTexPair** it;

    for (it = a; *it != NULL; it++) size_a++;
    for (it = b; *it != NULL; it++) size_b++;

    struct ShiftTexPair** new_tiles =
        HELPER_ALLOCATE_ARRAY(helper, struct ShiftTexPair*, size_a + size_b + 1);

    size_t index = 0;
    for (it = a; *it != NULL; it++) new_tiles[index++] = *it;
    for (it = b; *it != NULL; it++) new_tiles[index++] = *it;

    new_tiles[index] = NULL;
    free(a);
    free(b);
    return new_tiles;
}