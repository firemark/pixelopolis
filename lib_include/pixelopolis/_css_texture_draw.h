#pragma once
#include "pixelopolis/css_texture_draw.h"
#include "pixelopolis/draw_builder_texture.h"
#include "pixelopolis/draw_poly.h"
#include "pixelopolis/img.h"

// Basic
void css_texture_draw_default(struct TexObj *tex_obj, struct DrawTexInfo *info);

// Containers
void css_texture_draw_part(struct TexObj *tex_obj, struct DrawTexInfo *info);

#define TEX_OBJ_BASIC_TUPLE(tex_obj) tex_obj->basic.width, tex_obj->basic.height

#define MAKE_NEW_OUTPUT(tex_obj)                                                             \
    {                                                                                        \
        .texture = flat_image_create_with_color(TEX_OBJ_BASIC_TUPLE(tex_obj), &PURPLE),      \
        .normal_map = float_image_create_with_color(TEX_OBJ_BASIC_TUPLE(tex_obj), &FORWARD), \
    }

static inline void _output_copy(struct DrawTextureOutput *dst, struct DrawTextureOutput *src, int *pos) {
    flat_image_copy(dst->texture, src->texture, pos[0], pos[1]);
    float_image_copy(dst->normal_map, src->normal_map, pos[0], pos[1]);
}