#include "pixelopolis/_css_texture_draw.h"
#include "pixelopolis/draw_builder_texture.h"

void css_texture_draw_fill(struct TexObj *tex_obj, struct DrawTexInfo *info) {
    struct TexVoidObj *obj = tex_obj->obj;
    struct DrawTextureOutput child_output = MAKE_NEW_OUTPUT(obj->child);
    struct DrawTexInfo child_info = {.output = &child_output};
    css_texture_draw_component(obj->child, &child_info);
    _output_fill(info->output, &child_output);
    _output_clear(&child_output);
}