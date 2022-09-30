#include <stdlib.h>
#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/normal_map.h"
#include "pixelopolis/img.h"

#define _READ_TEXTURE(texture, filename, cache, type, func_read) \
    IMG_TYPE_##type* texture; { \
    texture = hash_get(cache, filename); \
    if (!texture) { \
        texture = func_read(filename); \
        if (texture) { \
            hash_set(css_builder_cache_textures, filename, texture, NULL); \
        } \
    } \
}

static struct FlatImage* _find_texture_in_rule(struct Rule* rule) {
    char* filename = css_find_string_prop(rule, "texture");
    if (!filename) {
        return NULL;
    }
    _READ_TEXTURE(texture, filename, css_builder_cache_textures, FLAT_IMAGE, flat_image_read_png_file);
    return texture;
}

static struct FloatImage* _find_bump_map_in_rule(struct Rule* rule) {
    char* filename = css_find_string_prop(rule, "bump-map");
    if (!filename) {
        return NULL;
    }
    _READ_TEXTURE(bump_map, filename, css_builder_cache_bump_maps, ONE_CHAN_IMAGE, one_chan_image_read_png_file);
    if (!bump_map) {
        return NULL;
    }
    struct FloatImage* normal_map = transform_bump_to_normal_map(bump_map);
    return normal_map;
}

static struct FloatImage* _find_normal_map_in_rule(struct Rule* rule) {
    return NULL;
    /* TODO
    char* filename = css_find_string_prop(rule, "normal-map");
    if (!filename) {
        return NULL;
    }
    _READ_TEXTURE(normal_map, filename, FLOAT_IMAGE, float_image_read_png_file);
    return normal_map;
    */
}

static struct rgb* _find_color_in_rule(struct Rule* rule) {
    return css_find_color_prop(rule, "color");
}

struct TexObj* builder_build_texture(struct SelectorHelper* helper) {
    struct Rule* rule = builder_make_rule_from_helper(helper);
    if (!rule) return NULL;

    struct TexObj* obj = malloc(sizeof(struct TexObj));
    obj->texture = _find_texture_in_rule(rule);
    obj->normal_map = _find_bump_map_in_rule(rule); // || _find_normal_map_in_rule(rule);
    obj->color = _find_color_in_rule(rule);

    css_free_rule_half(rule);
    return obj;
}
