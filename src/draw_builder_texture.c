#include "pixelopolis/draw_builder_texture.h"

#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/css.h"
#include "pixelopolis/css/func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/img.h"

struct HashMap* css_builder_cache_textures;
struct HashMap* css_builder_cache_bump_maps;
struct HashMap* css_builder_cache_normal_maps;

void builder_texture_init(void) {
    css_builder_cache_textures = hash_make();
    css_builder_cache_bump_maps = hash_make();
    css_builder_cache_normal_maps = hash_make();
}

void builder_texture_stop(void) {
    {
        struct FlatImage* texture;
        hash_iter_values(texture, css_builder_cache_textures) { flat_image_destroy(texture); }
        hash_destroy(css_builder_cache_textures);
    }

    {
        struct OneChanImage* bump_map;
        hash_iter_values(bump_map, css_builder_cache_bump_maps) {
            one_chan_image_destroy(bump_map);
        }
        hash_destroy(css_builder_cache_bump_maps);
    }

    {
        struct FloatImage* normal_map;
        hash_iter_values(normal_map, css_builder_cache_normal_maps) {
            float_image_destroy(normal_map);
        }
        hash_destroy(css_builder_cache_normal_maps);
    }
}

struct TexObj* builder_texture_make(struct Program* program, struct RuleWithParent* parent,
                                    struct RuleSelector* query, int width, int height) {
    struct TexVoidObj* obj = MEMORY_ALLOCATE(program->product_memory, struct TexVoidObj);
    struct TexObj* tex_obj = MEMORY_ALLOCATE(program->product_memory, struct TexObj);

    tex_obj->basic.width = width;
    tex_obj->basic.height = height;
    tex_obj->type = TEX_OBJ_VOID;
    tex_obj->obj = obj;
    tex_obj->parent = NULL;

    struct SelectorHelper helper = {
        .program = program,
        .parent_rule = parent,
        .selector = query,
        .parent = tex_obj,
    };

    obj->child = builder_texture_build_tex_obj(&helper);
    return tex_obj;
}
