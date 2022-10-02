#pragma once
#include "pixelopolis/css_func.h"
#include "pixelopolis/draw_builder.h"
#include "pixelopolis/hash.h"

struct TextureHelper {
    struct Program* program;
    struct Rule* rule;
    struct TexObj* parent;
};

struct TextureSelectorHelper {
    struct Program* program;
    struct Rule* parent_rule;
    struct RuleSelector* selector;
    struct TexObj* parent;
};

#define make_texture_selector_helper(helper, prop) { \
    .program=helper->program, \
    .parent_rule=helper->rule, \
    .selector=css_find_selector_prop(helper->rule, prop), \
    .parent=helper->parent, \
}

// Builders
struct TexObj* builder_texture_build(struct TextureSelectorHelper* helper);
struct TexObj* builder_texture_build_default(struct TextureHelper* helper);
struct TexObj* builder_texture_build_tiles(struct TextureHelper* helper);
// struct TexObj* builder_texture_build_bricks(struct TextureHelper* helper);

// Utils
struct Rule* builder_texture_make_rule_from_helper(struct TextureSelectorHelper* helper);

// Caches
extern struct HashMap* css_builder_cache_textures;
extern struct HashMap* css_builder_cache_bump_maps;
extern struct HashMap* css_builder_cache_normal_maps;