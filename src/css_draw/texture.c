#include "pixelopolis/_css_draw.h"

struct rgb PURPLE = {.r = 0xFF, .g = 0x00, .b = 0xFF};
struct xyz FORWARD = {.x = 0.0f, .y = 0.0f, .z = 1.0f};

int _get_height_of_floor(struct FloorObj* obj) {
    int height = obj->height;

    struct TexObj* tex_obj = obj->tex;
    if (!tex_obj) return height;
    struct FlatImage* texture = tex_obj->texture;
    if (!texture) return height;

    return height ? height : texture->height;
}

void _draw_floor_on_texture(struct DrawTextureOutput* output, struct FloorObj* obj, int height,
                            enum Valign valign) {
    if (valign == VALIGN_TOP) {
        height -= _get_height_of_floor(obj);
    }

    struct TexObj* tex_obj = obj->tex;
    if (tex_obj) {
        if (tex_obj->texture) {
            flat_image_fill_column(output->texture, tex_obj->texture, height);
        }

        if (tex_obj->normal_map) {
            float_image_fill_column(output->normal_map, tex_obj->normal_map, height);
        }

        if (tex_obj->color && obj->height > 0) {
            struct FlatImage* texture =
                flat_image_create_with_color(output->texture->width, obj->height, tex_obj->color);
            flat_image_copy(output->texture, texture, 0, height);
            flat_image_destroy(texture);
        }
    }

    size_t obj_index;
    for (obj_index = 0; obj_index < obj->objs_size; obj_index++) {
        struct ShiftTexPair* pair = obj->objs[obj_index];
        struct TexObj* tex = pair->obj;
        int shift = pair->shift;
        if (!tex) continue;
        if (tex->texture) {
            flat_image_copy(output->texture, tex->texture, shift, height);
        }
        if (tex->normal_map) {
            float_image_copy(output->normal_map, tex->normal_map, shift, height);
        }
    }
}

void css_draw_make_texture_from_wall(struct DrawTextureOutput* output, struct WallObj* obj,
                                     int width, int height) {
    output->texture = flat_image_create_with_color(width, height, &PURPLE);
    output->normal_map = float_image_create_with_color(width, height, &FORWARD);
    struct TexObj* tex_obj = obj->tex;
    if (tex_obj) {
        if (tex_obj->texture) {
            flat_image_fill(output->texture, tex_obj->texture);
        }
        if (tex_obj->normal_map) {
            float_image_fill(output->normal_map, tex_obj->normal_map);
        }
        if (tex_obj->color) {
            struct FlatImage* texture = flat_image_create_with_color(width, height, tex_obj->color);
            flat_image_fill(output->texture, texture);
            flat_image_destroy(texture);
        }
    }

    int max_height = height;
    int start_height = 0;

    struct FloorObj* bottom = obj->bottom;
    if (bottom) {
        _draw_floor_on_texture(output, bottom, start_height, VALIGN_BOTTOM);
        start_height += _get_height_of_floor(bottom) + obj->padding;
    }

    struct FloorObj* top = obj->top;
    if (top) {
        _draw_floor_on_texture(output, top, max_height, VALIGN_TOP);
    }

    int floor_index;
    for (floor_index = 0; floor_index < obj->floors_size; floor_index++) {
        struct FloorObj* middle = obj->floors[floor_index];
        int tex_height = 0;
        if (middle) {
            _draw_floor_on_texture(output, middle, start_height, VALIGN_BOTTOM);
            tex_height = _get_height_of_floor(middle);
        }
        start_height += tex_height + obj->padding;
    }
}

static inline void _scale_fill_pixel(struct FlatImage* img, struct rgb* color, const int x,
                                     const int y, const size_t n) {
    int i, j;
    int width = img->width;
    for (i = 0; i < n; i++) {
        size_t coord = (y * n + i) * width + x * n;
        for (j = 0; j < n; j++) {
            img->buffer[coord + j] = *color;
        }
    }
}

static inline struct FlatImage* _scaleN(struct FlatImage* img, const size_t n) {
    int old_width = img->width;
    int old_height = img->height;
    int width = old_width * n;
    int height = old_height * n;
    struct FlatImage* new_img = flat_image_create(width, height);
    int x, y;
    for (y = 0; y < old_height; y++) {
        for (x = 0; x < old_width; x++) {
            size_t old_coord = y * old_width + x;
            struct rgb* color = &img->buffer[old_coord];
            _scale_fill_pixel(new_img, color, x, y, n);
        }
    }
    return new_img;
}

static inline struct FlatImage* _scale_mame2(struct FlatImage* img) {
    int old_width = img->width;
    int old_height = img->height;
    int width = old_width * 2;
    int height = old_height * 2;
    struct FlatImage* new_img = flat_image_create(width, height);
    int x, y;
    // each pixel except border
    for (y = 1; y < old_height - 1; y++) {
        for (x = 1; x < old_width - 1; x++) {
            // https://en.wikipedia.org/wiki/Pixel-art_scaling_algorithms#2×SaI
            struct rgb pp = img->buffer[(y + 0) * old_width + x + 0];
            struct rgb pa = img->buffer[(y + 1) * old_width + x + 0];
            struct rgb pb = img->buffer[(y + 0) * old_width + x + 1];
            struct rgb pc = img->buffer[(y + 0) * old_width + x - 1];
            struct rgb pd = img->buffer[(y - 1) * old_width + x + 0];
#define CMP_COLOR(pa, pb) (pa.r == pb.r && pa.g == pb.g && pa.b == pb.b)
            char ab_eq = CMP_COLOR(pa, pb);
            char ac_eq = CMP_COLOR(pa, pc);
            char cd_eq = CMP_COLOR(pc, pd);
            char bd_eq = CMP_COLOR(pb, pd);
#undef CMP_COLOR
            struct rgb p1 = ac_eq && !cd_eq && !ab_eq ? pa : pp;
            struct rgb p2 = ab_eq && !ac_eq && !bd_eq ? pb : pp;
            struct rgb p3 = cd_eq && !bd_eq && !ac_eq ? pc : pp;
            struct rgb p4 = bd_eq && !ab_eq && !cd_eq ? pd : pp;

            int yy = y * 2;
            int xx = x * 2;

            new_img->buffer[(yy + 0) * width + xx + 0] = p1;
            new_img->buffer[(yy + 1) * width + xx + 0] = p2;
            new_img->buffer[(yy + 0) * width + xx + 1] = p3;
            new_img->buffer[(yy + 1) * width + xx + 1] = p4;
        }
    }

    // bottom border
    for (x = 0; x < old_width; x++) {
        size_t old_coord = x;
        struct rgb* color = &img->buffer[old_coord];
        _scale_fill_pixel(new_img, color, x, 0, 2);
    }

    // top border
    for (x = 0; x < old_width; x++) {
        size_t old_coord = (old_height - 1) * old_width + x;
        struct rgb* color = &img->buffer[old_coord];
        _scale_fill_pixel(new_img, color, x, old_height - 1, 2);
    }

    // left border
    for (y = 0; y < old_height; y++) {
        size_t old_coord = y * old_width;
        struct rgb* color = &img->buffer[old_coord];
        _scale_fill_pixel(new_img, color, 0, y, 2);
    }

    // right border
    for (y = 0; y < old_height; y++) {
        size_t old_coord = y * old_width + old_width - 1;
        struct rgb* color = &img->buffer[old_coord];
        _scale_fill_pixel(new_img, color, old_width - 1, y, 2);
    }

    return new_img;
}

static inline struct FlatImage* _scale_mame4(struct FlatImage* img) {
    struct FlatImage* mame2 = _scale_mame2(img);
    struct FlatImage* mame4 = _scale_mame2(mame2);
    flat_image_destroy(mame2);
    return mame4;
}

struct FlatImage* css_draw_scale_image(struct FlatImage* img, enum TexFilterAlgorithm filter) {
    if (!img) return NULL;
    switch (filter) {
        case FILTER_ALGORITHM_SCALE2:
            return _scaleN(img, 2);
        case FILTER_ALGORITHM_SCALE3:
            return _scaleN(img, 3);
        case FILTER_ALGORITHM_MAME2:
            return _scale_mame2(img);
        case FILTER_ALGORITHM_MAME4:
            return _scale_mame4(img);
        default:
            return NULL;  // don't filter
    }
}

void css_draw_texture(struct DrawTextureOutput* output, struct DrawTexInfo* info) {
    css_draw_make_texture_from_wall(output, info->wall, info->size[0], info->size[1]);
    // struct FlatImage* filtered_img = css_draw_scale_image(img, info->filter);
    // if (!filtered_img) return img;
    // flat_image_destroy(img);
}
