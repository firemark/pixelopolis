#pragma once

#include "pixelopolis/_draw_builder_utils.h"
#include "pixelopolis/draw_builder_texture.h"
#include "pixelopolis/hash.h"

#define BUILDER_TEXTURE_MAX_ELEMENTS 512

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

struct IntPair {
    int start;
    int end;
};

struct Rule* builder_texture_make_rule_from_helper(struct SelectorHelper* helper);

// Builders.
struct TexObj* builder_texture_build_tex_obj(struct SelectorHelper* helper);
struct TexObj* builder_texture_build_default(struct Helper* helper);

// Builder containers.
struct TexObj* builder_texture_build_tile(struct Helper* helper);
struct TexObj* builder_texture_build_texture_part(struct Helper* helper, enum TexPartDirection);
struct TexObj* builder_texture_build_texture_series(struct Helper* helper, enum TexPartDirection);

// Part utils.
void builder_texture_align(struct Helper* helper, struct ShiftTexPair** pairs,
                           struct BasicTexObj* basic, struct IntPair length, struct IntPair index,
                           enum TexPartDirection direction);
void builder_texture_justify(struct Helper* helper, struct ShiftTexPair** pairs,
                             struct BasicTexObj* basic, enum TexPartDirection direction);

// Utils.
struct BasicTexObj builder_texture_prepare_basic(struct Helper* helper);
struct TexObj* builder_texture_make_tex_obj(struct Helper* helper, struct BasicTexObj basic,
                                            enum TexObjType type, void* obj);

// Caches.
extern struct HashMap* css_builder_cache_textures;
extern struct HashMap* css_builder_cache_bump_maps;
extern struct HashMap* css_builder_cache_normal_maps;