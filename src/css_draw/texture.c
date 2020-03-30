#include "_css_draw.h"

struct rgb PURPLE = { .r=0xFF, .g=0x00, .b=0xFF };

int _get_height_of_floor(struct FloorObj* obj) {
    int height = obj->height;

    struct TexObj* tex_obj = obj->tex;
    if (!tex_obj) return height;
    struct FlatImage* texture = tex_obj->texture;
    if (!texture) return height;

    return height ? height : texture->height;
}

void _draw_floor_on_texture(struct FlatImage *img, struct FloorObj *obj, int height, enum Valign valign) {
    if (valign == VALIGN_TOP) {
        height -= _get_height_of_floor(obj);
    }

    struct TexObj* tex_obj = obj->tex;
    if (!tex_obj) goto without_texture; 
    struct FlatImage* texture = tex_obj->texture;
    if (!texture) goto without_texture;
    flat_image_fill_column(img, texture, height);

without_texture:;
    size_t obj_index;
    for(obj_index = 0; obj_index < obj->objs_size; obj_index++) {
        struct ShiftTexPair* pair = obj->objs[obj_index];
        struct TexObj* tex = pair->obj;
        int shift = pair->shift;
        if (!tex || !tex->texture) continue;
        flat_image_copy(img, tex->texture, shift, height);
    }
}

struct FlatImage* css_draw_make_texture_from_wall(struct WallObj *obj, int width, int height) {
    struct FlatImage *img = flat_image_create_with_color(width, height, &PURPLE);
    struct TexObj *tex_obj = obj->tex;
    if (tex_obj) {
        if (tex_obj->texture) {
            flat_image_fill(img, tex_obj->texture);
        }
        if (tex_obj->color) {
            struct FlatImage* texture = flat_image_create_with_color(width, height, tex_obj->color);
            flat_image_fill(img, texture);
            flat_image_destroy(texture);
        }
    }

    int max_height = height;
    int start_height = 0;
    
    struct FloorObj *bottom = obj->bottom;
    if (bottom) {
        _draw_floor_on_texture(img, bottom, start_height, VALIGN_BOTTOM);
        start_height += _get_height_of_floor(bottom) + obj->padding;
    }

    struct FloorObj *top = obj->top;
    if (top) {
        _draw_floor_on_texture(img, top, max_height, VALIGN_TOP);
    }

    int floor_index;
    for(floor_index = 0; floor_index < obj->floors_size; floor_index++) {
        struct FloorObj *middle = obj->floors[floor_index];
        int tex_height = 0;
        if (middle) {
            _draw_floor_on_texture(img, middle, start_height, VALIGN_BOTTOM);
            tex_height = _get_height_of_floor(middle);
        } else {
            tex_height = 12;
        }
        start_height += tex_height + obj->padding;
    }

    return img;
}
