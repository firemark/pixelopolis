#pragma once
#include <math.h>

#include "pixelopolis/_draw_builder_utils.h"
#include "pixelopolis/draw_builder_texture.h"
#include "pixelopolis/hash.h"

#define BUILDER_TEXTURE_MAX_ELEMENTS 128

struct Helper {
    struct Program* program;
    struct Rule* rule;
    struct TexObj* parent;
};

struct SelectorHelper {
    struct Program* program;
    struct Rule* parent_rule;
    struct RuleSelector* selector;
    struct TexObj* parent;
};

struct Rule* builder_texture_make_rule_from_helper(struct SelectorHelper* helper);

// Builders.
struct TexObj* builder_texture_build_tex_obj(struct SelectorHelper* helper);
struct TexObj* builder_texture_build_default(struct Helper* helper);
struct TexObj* builder_texture_build_tile(struct Helper* helper);

// Builder containers.
struct TexObj* builder_texture_build_texture_part(struct Helper* helper, enum TexPartDirection);

// Utils.
struct TexObj* builder_texture_make_tex_obj(struct Helper* helper, struct BasicTexObj basic,
                                            enum TexObjType type, void* obj);

// Caches.
extern struct HashMap* css_builder_cache_textures;
extern struct HashMap* css_builder_cache_bump_maps;
extern struct HashMap* css_builder_cache_normal_maps;