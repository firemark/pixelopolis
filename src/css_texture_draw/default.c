#include "pixelopolis/_css_texture_draw.h"
#include "pixelopolis/draw_builder_texture.h"

void css_texture_draw_default(struct TexObj *tex_obj, struct DrawTexInfo *info) {
    struct TexDefaultObj *obj = tex_obj->obj;
    if (obj->texture) {
        flat_image_fill(info->output->texture, obj->texture);
    }
    if (obj->normal_map) {
        float_image_fill(info->output->normal_map, obj->normal_map);
    }
    if (obj->color) {
        int width = info->output->texture->width;
        int height = info->output->texture->height;
        struct FlatImage *texture = flat_image_create_with_color(width, height, obj->color);
        flat_image_fill(info->output->texture, texture);
        flat_image_destroy(texture);
    }
}