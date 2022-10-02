#include <stdlib.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/img/create.h"
#include "pixelopolis/img/ops.h"

struct TexObj* builder_texture_build_tiles(struct TextureHelper* helper) {
    struct Rule* rule = helper->rule;
    struct TexObj* obj = malloc(sizeof(struct TexObj));

    int* tile_width = css_find_number_prop(rule, "tile-width");
    if (!tile_width) return NULL;

    int* tile_height = css_find_number_prop(rule, "tile-height");
    if (!tile_height) return NULL;

    struct rgb* first_color = css_find_color_prop(rule, "first-color");
    if (!first_color) return NULL;

    struct rgb* second_color = css_find_color_prop(rule, "second-color");
    if (!second_color) return NULL;

    struct FlatImage* texture = flat_image_create(*tile_width * 2, *tile_height * 2);

    int y, x;
    for (y = 0; y < *tile_height; y++) {
        for (x = 0; x < *tile_width; x++) {
            IMG_PIXEL(texture, x, y) = *first_color;
            IMG_PIXEL(texture, x + *tile_width, y) = *second_color;
            IMG_PIXEL(texture, x, y + *tile_height) = *second_color;
            IMG_PIXEL(texture, x + *tile_width, y + *tile_height) = *first_color;
        }
    }

    obj->texture = texture;
    obj->normal_map = NULL;
    obj->color = NULL;
    return obj;
}