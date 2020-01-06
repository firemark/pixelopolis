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


struct TexObj* builder_build_texture(struct SelectorHelper* helper) {
    struct Rule* rule = builder_make_rule_from_helper(helper);
    if (!rule) return NULL;
    char* filename = css_find_string_prop(rule, "texture"); 
    if (!filename) return NULL;

    struct TexObj* obj = malloc(sizeof(struct TexObj));
    obj->texture = filename ? _builder_get_texture(filename) : NULL;

    css_free_rule_half(rule);
    return obj;
}
