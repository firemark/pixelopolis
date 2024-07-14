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

struct IntPair {
    int start;
    int end;
};

struct Rule* builder_texture_make_rule_from_helper(struct SelectorHelper* helper);

// Builders.
struct TexObj* builder_texture_build_tex_obj(struct SelectorHelper* helper);
struct TexObj* builder_texture_build_default(struct Helper* helper);
struct TexObj* builder_texture_build_tile(struct Helper* helper);

// Builder containers.
struct TexObj* builder_texture_build_texture_part(struct Helper* helper, enum TexPartDirection);

// Part utils.
void builder_texture_align(struct Helper* helper, struct TexPartObj* obj, struct BasicTexObj* basic,
                           struct IntPair length, struct IntPair index,
                           enum TexPartDirection direction);
void builder_texture_justify(struct Helper* helper, struct TexPartObj* obj,struct BasicTexObj* basic,
                             enum TexPartDirection direction);

// Utils.
struct BasicTexObj builder_texture_prepare_basic(struct Helper* helper);
struct TexObj* builder_texture_make_tex_obj(struct Helper* helper, struct BasicTexObj basic,
                                            enum TexObjType type, void* obj);

static inline int builder_texture_get_metric_by_direction(struct BasicTexObj* basic,
                                                          enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            return basic->width;
        case TEX_PART_HORIZONTAL:
            return basic->height;
        default:
            return 0;
    }
}

static inline int builder_texture_get_cometric_by_direction(struct BasicTexObj* basic,
                                                            enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            return basic->height;
        case TEX_PART_HORIZONTAL:
            return basic->width;
        default:
            return 0;
    }
}

static inline int builder_texture_get_pair_axis_by_direction(enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            return 0;
        case TEX_PART_HORIZONTAL:
            return 1;
        default:
            return 0;
    }
}

static inline int builder_texture_get_pair_coaxis_by_direction(enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            return 1;
        case TEX_PART_HORIZONTAL:
            return 0;
        default:
            return 0;
    }
}

static inline void builder_texture_resize_axis_by_direction(struct BasicTexObj* a, int len,
                                                            enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            a->width = a->width >= len ? a->width : len;
            break;
        case TEX_PART_HORIZONTAL:
            a->height = a->height >= len ? a->height : len;
            break;
    }
}

// Caches.
extern struct HashMap* css_builder_cache_textures;
extern struct HashMap* css_builder_cache_bump_maps;
extern struct HashMap* css_builder_cache_normal_maps;