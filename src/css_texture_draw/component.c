#include "pixelopolis/_css_texture_draw.h"

void css_texture_draw_component(struct TexObj* tex_obj, struct DrawTexInfo* info) {
    if (!tex_obj) return;
    switch (tex_obj->type) {
        case TEX_OBJ_DEFAULT:
            css_texture_draw_default(tex_obj, info);
            break;
        case TEX_OBJ_PART:
            css_texture_draw_part(tex_obj, info);
            break;
        case TEX_OBJ_TILES:
            // css_texture_draw_tiles(obj, &inner_info);
            break;
        case TEX_OBJ_VOID:
            css_texture_draw_void(tex_obj, info);
            break;
    }
}
