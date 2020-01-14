#include <string.h>

#include "_draw_builder.h"

#include "img.h"
#include "css_func.h"


struct HashMap* css_builder_cache_textures;

void builder_init(void) {
    css_builder_cache_textures = hash_make();
}

void builder_stop(void) {
    struct FlatImage* texture;
    hash_iter_values(texture, css_builder_cache_textures) {
        flat_image_destroy(texture);
    }
    hash_destroy(css_builder_cache_textures);
}

struct DrawObj* builder_make(struct Program* program, struct Rule* world) {
    struct RuleSelector* query = css_find_selector_prop(world, "body");
    struct SelectorHelper helper = {
        .program=program,
        .parent_rule=NULL,
        .selector=query,
        .parent=NULL,
    };
    return builder_build_draw_obj(&helper);
}
