#include <string.h>

#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/img.h"

struct HashMap* css_builder_cache_textures;
struct HashMap* css_builder_cache_bump_maps;
struct HashMap* css_builder_cache_normal_maps;

void builder_init(void) {
    css_builder_cache_textures = hash_make();
    css_builder_cache_bump_maps = hash_make();
    css_builder_cache_normal_maps = hash_make();
}

void builder_stop(void) {
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

struct DrawObj* builder_make(struct Program* program, struct Rule* world) {
    struct RuleSelector* query = css_find_selector_prop(world, "body");
    struct SelectorHelper helper = {
        .program = program,
        .parent_rule = NULL,
        .selector = query,
        .parent = NULL,
    };
    return builder_build_draw_obj(&helper);
}
