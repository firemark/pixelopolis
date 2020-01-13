#include <stdlib.h>
#include "_draw_builder.h"
#include "css_func.h"
#include "img.h"

struct FlatImage* _builder_get_texture(char *filename) {
    struct FlatImage* texture;

    texture = hash_get(css_builder_cache_textures, filename);
    if (texture) {
        return texture;
    }

    texture = flat_image_read_png_file(filename);
    if (!texture) {
        return NULL;
    }

    hash_set(css_builder_cache_textures, filename, texture, NULL);
    return texture;
}


struct FlatImage* _find_texture_in_rule(struct Rule* rule) {
    char* filename = css_find_string_prop(rule, "texture");
    return filename ? _builder_get_texture(filename) : NULL;
}

struct rgb* _find_color_in_rule(struct Rule* rule) {
    return css_find_color_prop(rule, "color");
}

struct TexObj* builder_build_texture(struct SelectorHelper* helper) {
    struct Rule* rule = builder_make_rule_from_helper(helper);
    if (!rule) return NULL;

    struct TexObj* obj = malloc(sizeof(struct TexObj));
    obj->texture = _find_texture_in_rule(rule);
    obj->color = _find_color_in_rule(rule);

    css_free_rule_half(rule);
    return obj;
}
