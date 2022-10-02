#include <stdlib.h>

#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/_draw_builder_texture.h"

struct TexObj* builder_build_texture(struct SelectorHelper* helper) {
    struct TextureSelectorHelper texture_helper = {
        .program = helper->program,
        .parent_rule = helper->parent_rule,
        .selector = helper->selector,
        .parent = NULL,
    };

    return builder_texture_build(&texture_helper);
}
