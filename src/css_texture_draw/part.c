#include "pixelopolis/_css_texture_draw.h"
#include "pixelopolis/draw_builder_texture.h"

void css_texture_draw_part(struct TexObj *tex_obj, struct DrawTexInfo *info) {
    struct TexPartObj *obj = tex_obj->obj;
    if (!obj) return;
    if (obj->background) {
        css_texture_draw_component(obj->background, info);
    }

    if (!obj->objs) return;
    struct ShiftTexPair *pair = NULL;
    size_t index = 0;
    while ((pair = obj->objs[index++])) {
        struct DrawTextureOutput pair_output = MAKE_NEW_OUTPUT(pair->obj);
        struct DrawTexInfo pair_info = {.output = &pair_output};
        css_texture_draw_component(pair->obj, &pair_info);
        _output_copy(info->output, &pair_output, pair->shift);
    }

}