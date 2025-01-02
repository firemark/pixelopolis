#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/basic.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/img/create.h"
#include "pixelopolis/img/read.h"
#include "pixelopolis/normal_map.h"

static struct FlatImage* _find_texture_in_rule(struct Rule* rule);
static struct OneChanImage* _find_bump_map_in_rule(struct Rule* rule);
static void _bevel(struct Rule* rule, struct OneChanImage* bump_map);
static void _bump_noise(struct Rule* rule, struct OneChanImage* bump_map);
static struct rgb* _find_color_in_rule(struct Rule* rule);

#define SET_GREATER(basic, value, attr) basic.attr = basic.attr >= value ? basic.attr : value

struct TexObj* builder_texture_build_default(struct Helper* helper) {
    struct Rule* rule = helper->rule;
    struct OneChanImage* bump_map = _find_bump_map_in_rule(rule);
    struct BasicTexObj basic = builder_texture_prepare_basic(helper);

    struct TexDefaultObj* obj = malloc(sizeof(struct TexDefaultObj));
    obj->texture = _find_texture_in_rule(rule);
    obj->color = _find_color_in_rule(rule);
    // TODO - points texture

    if (obj->texture) {
        SET_GREATER(basic, obj->texture->width, width);
        SET_GREATER(basic, obj->texture->height, height);
    } else if (bump_map) {
        SET_GREATER(basic, bump_map->width, width);
        SET_GREATER(basic, bump_map->height, height);
    } else {
        SET_GREATER(basic, 1, width);
        SET_GREATER(basic, 1, height);
    }

    if (!bump_map) {
        bump_map = one_chan_image_create_with_color(basic.width, basic.height, 0xFF);
    }
    _bump_noise(rule, bump_map);
    _bevel(rule, bump_map);
    obj->normal_map = transform_bump_to_normal_map(bump_map);

    struct TexObj* tex_obj = malloc(sizeof(struct TexObj));
    tex_obj->type = TEX_OBJ_DEFAULT;
    tex_obj->obj = obj;
    tex_obj->basic = basic;

    return tex_obj;
}

#define _READ_TEXTURE(texture, filename, cache, type, func_read)               \
    IMG_TYPE_##type* texture;                                                  \
    {                                                                          \
        texture = hash_get(cache, filename);                                   \
        if (!texture) {                                                        \
            texture = func_read(filename);                                     \
            if (texture) {                                                     \
                hash_set(css_builder_cache_textures, filename, texture, NULL); \
            }                                                                  \
        }                                                                      \
    }

static struct FlatImage* _find_texture_in_rule(struct Rule* rule) {
    char* filename = css_find_string_prop(rule, "texture");
    if (!filename) {
        return NULL;
    }
    _READ_TEXTURE(texture, filename, css_builder_cache_textures, FLAT_IMAGE,
                  flat_image_read_png_file);
    return texture;
}

static struct OneChanImage* _find_bump_map_in_rule(struct Rule* rule) {
    char* filename = css_find_string_prop(rule, "bump-map");
    if (!filename) {
        return NULL;
    }
    _READ_TEXTURE(bump_map, filename, css_builder_cache_bump_maps, ONE_CHAN_IMAGE,
                  one_chan_image_read_png_file);
    return bump_map;
}

static void _bevel(struct Rule* rule, struct OneChanImage* bump_map) {
    int* bevel = css_find_number_prop(rule, "bevel");
    if (!bevel || *bevel == 0) {
        return;
    }
    const uint8_t _bevel = 0xFF - (uint8_t)*bevel;
    const int y0 = 0;
    const int y1 = bump_map->height - 1;
    const int x0 = 0;
    const int x1 = bump_map->width - 1;

    for (int x = 0; x < bump_map->width; x++) {
        bump_map->buffer[y0 * bump_map->width + x] = _bevel;
        bump_map->buffer[y1 * bump_map->width + x] = _bevel;
    }

    for (int y = 0; y < bump_map->height; y++) {
        bump_map->buffer[y * bump_map->width + x0] = _bevel;
        bump_map->buffer[y * bump_map->width + x1] = _bevel;
    }
}

static void _bump_noise(struct Rule* rule, struct OneChanImage* bump_map) {
    int* noise = css_find_percent_prop(rule, "bump-noise");
    int* level = css_find_number_prop(rule, "bump-noise-level");
    if (!noise || *noise == 0) {
        return;
    }
    const uint8_t c_level = level ? (uint8_t)*level : 0xFF;
    if (c_level == 0) {
        return;
    }
    size_t count = (bump_map->width * bump_map->height * *noise) / 100;
    for (size_t index = 0; index < count; index++) {
        const int x = rand() % bump_map->width;
        const int y = rand() % bump_map->width;
        const uint8_t c = rand() % c_level;
        bump_map->buffer[y * bump_map->width + x] = 0xFF - c;
    }
}

static struct rgb* _find_color_in_rule(struct Rule* rule) {
    return css_find_color_prop(rule, "color");
}