#include "pixelopolis/_css_texture_draw.h"
#include "pixelopolis/draw_builder_texture.h"

void css_texture_draw_void(struct TexObj *tex_obj, struct DrawTexInfo *info) {
    struct TexVoidObj* obj = tex_obj->obj;
    css_texture_draw_component(obj->child, info);
}