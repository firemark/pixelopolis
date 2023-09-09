#pragma once
#include <math.h>

#include "pixelopolis/draw_builder_texture.h"
#include "pixelopolis/hash.h"

#include "pixelopolis/_draw_builder_utils.h"

#define BUILDER_TEXTURE_MAX_ELEMENTS 128

// Builders.
struct TexObj* builder_texture_build_tex_obj(struct SelectorHelper* helper);
struct TexObj* builder_texture_build_default(struct Helper* helper);
struct TexObj* builder_texture_build_tile(struct Helper* helper);

// Builder containers.
struct TexObj* builder_texture_build_texture_part(struct Helper* helper, enum TexObjType type, enum TexPartDirection);

// Caches.
extern struct HashMap* css_builder_cache_textures;
extern struct HashMap* css_builder_cache_bump_maps;
extern struct HashMap* css_builder_cache_normal_maps;