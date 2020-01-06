#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "css_draw.h"
#include "css_func.h"
#include "draw_poly.h"
#include "hash.h"
#include "img.h"
#include "draw_builder.h"

struct DrawInnerInfo {
    struct image *img; 
    int *vox;
    int *out_vox;
};

enum Valign { VALIGN_TOP, VALIGN_BOTTOM };

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

struct FlatImage* _make_texture_from_wall(struct WallObj *obj, int width, int height) {
    struct FlatImage *img = flat_image_create(width, height);
    struct TexObj *tex_obj = obj->tex;
    if (tex_obj) {
        flat_image_fill(img, obj->tex->texture);
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

void _css_draw_plane(struct image* img, struct FlatImage* img_to_draw, int voxes[12]) {
    /* 2---3
     * |B /|
     * | / |
     * |/ A|
     * 0---1 
     */
    int w = img_to_draw->width;
    int h = img_to_draw->height;
#define VEC(n) voxes[(n) * 3 + 0], voxes[(n) * 3 + 1], voxes[(n) * 3 + 2]
    int voxes_a[9] = { VEC(0), VEC(1), VEC(3) };
    int voxes_b[9] = { VEC(3), VEC(2), VEC(0) };
#undef VEC
    int uv_a[6] = {
        0, 0, // 0
        w, 0, // 1
        w, h, // 3
    };
    int uv_b[6] = {
        w, h, // 3
        0, h, // 2
        0, 0, // 0
    };

    draw_poly(img, img_to_draw, voxes_a, uv_a);
    draw_poly(img, img_to_draw, voxes_b, uv_b);
}

void _css_draw_void(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct VoidObj *obj = draw_obj->obj;
    if (!obj) return;
    struct DrawObj *child = obj->child;
    if (!child) return;
    int *out_vox = inner_info->out_vox;

    struct DrawInfo draw_info = {
        .img=inner_info->img,
        .vox=inner_info->vox,
    };
    draw_component(child, &draw_info, out_vox);

    if (out_vox) {
        int width = draw_obj->basic.width;
        int height = draw_obj->basic.height;
        int depth = draw_obj->basic.depth;
        out_vox[0] += width;
        out_vox[1] += depth;
        out_vox[2] += height;
    }
}

void _css_draw_cube(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct CubeObj *obj = draw_obj->obj;
    if (!obj) return;
    int *vox = inner_info->vox;
    int width = draw_obj->basic.width;
    int height = draw_obj->basic.height;
    int depth = draw_obj->basic.depth;
    int w = width;
    int h = height;
    int d = depth;

    struct WallObj *east_wall_obj = obj->east_wall;
    if (east_wall_obj && depth > 0 && height > 0) {
        int voxes_first[12] = {
            vox[0] + w, vox[1] + 0, vox[2] + 0,
            vox[0] + w, vox[1] + d, vox[2] + 0,
            vox[0] + w, vox[1] + 0, vox[2] + h,
            vox[0] + w, vox[1] + d, vox[2] + h,
        };
        int voxes_second[12] = {
            vox[0] + 0, vox[1] + 0, vox[2] + 0,
            vox[0] + 0, vox[1] + d, vox[2] + 0,
            vox[0] + 0, vox[1] + 0, vox[2] + h,
            vox[0] + 0, vox[1] + d, vox[2] + h,
        };
        struct FlatImage* img_to_draw = _make_texture_from_wall(east_wall_obj, depth, height);
        _css_draw_plane(inner_info->img, img_to_draw, voxes_first);
        _css_draw_plane(inner_info->img, img_to_draw, voxes_second);
        free(img_to_draw);
    }

    struct WallObj *south_wall_obj = obj->south_wall;
    if (south_wall_obj && width > 0 && height > 0) {
        int voxes[12] = {
            vox[0] + 0, vox[1], vox[2] + 0,
            vox[0] + w, vox[1], vox[2] + 0,
            vox[0] + 0, vox[1], vox[2] + h,
            vox[0] + w, vox[1], vox[2] + h,
        };
        struct FlatImage* img_to_draw = _make_texture_from_wall(south_wall_obj, width, height);
        _css_draw_plane(inner_info->img, img_to_draw, voxes);
        free(img_to_draw);
    }

    struct WallObj *roof_obj = obj->roof;
    if (roof_obj && width > 0 && depth > 0) {
        int voxes[12] = {
            vox[0] + 0, vox[1] + 0, vox[2] + h,
            vox[0] + w, vox[1] + 0, vox[2] + h,
            vox[0] + 0, vox[1] + d, vox[2] + h,
            vox[0] + w, vox[1] + d, vox[2] + h,
        };
        struct FlatImage* img_to_draw = _make_texture_from_wall(roof_obj, width, depth);
        _css_draw_plane(inner_info->img, img_to_draw, voxes);
        free(img_to_draw);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] += width;
        out_vox[1] += depth;
        out_vox[2] += height;
    }
}

void _css_draw_wide_triangle(struct image* img, struct FlatImage* img_to_draw, int voxes[9]) {
    /*     2
     *    / \
     *   /   \
     *  /     \
     * 0-------1
     */
    int h = img_to_draw->height;
    int w = img_to_draw->width;
    int wh = w / 2;
    int uv[6] = {
        0, 0, // 0
        w, 0, // 1
        wh, h // 2
    }; 
    draw_poly(img, img_to_draw, voxes, uv);
}

void _css_draw_triangle(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct TriangleObj *obj = draw_obj->obj;
    if (!obj) return;
    int *vox = inner_info->vox;
    int width = draw_obj->basic.width;
    int height = draw_obj->basic.height;
    int depth = draw_obj->basic.depth;
    int w = width;
    int wh = width / 2;
    int h = height;
    int d = depth;

    struct WallObj *wall_obj = obj->wall;
    if (wall_obj && width > 0 && height > 0) {
        int voxes[9] = {
            vox[0] + 0 , vox[1], vox[2] + 0,
            vox[0] + w , vox[1], vox[2] + 0,
            vox[0] + wh, vox[1], vox[2] + h,
        };
        struct FlatImage* img_to_draw = _make_texture_from_wall(wall_obj, width, height);
        _css_draw_wide_triangle(inner_info->img, img_to_draw, voxes);
        free(img_to_draw);
    }

    struct WallObj *roof_obj = obj->roof;
    if (roof_obj) {
        struct FlatImage* img_to_draw_first = _make_texture_from_wall(roof_obj, width, height);
        struct FlatImage* img_to_draw_second = _make_texture_from_wall(roof_obj, width, height);

        int voxes_first[12] = {
            vox[0] + 0 , vox[1] + 0, vox[2] + 0,
            vox[0] + 0 , vox[1] + d, vox[2] + 0,
            vox[0] + wh, vox[1] + 0, vox[2] + h,
            vox[0] + wh, vox[1] + d, vox[2] + h,
        };
        _css_draw_plane(inner_info->img, img_to_draw_first, voxes_first);

        int voxes_second[12] = {
            vox[0] + w , vox[1] + 0, vox[2] + 0,
            vox[0] + w , vox[1] + d, vox[2] + 0,
            vox[0] + wh, vox[1] + 0, vox[2] + h,
            vox[0] + wh, vox[1] + d, vox[2] + h,
        };
        _css_draw_plane(inner_info->img, img_to_draw_second, voxes_second);

        free(img_to_draw_first);
        free(img_to_draw_second);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] += width;
        out_vox[1] += depth;
        out_vox[2] += height;
    }
}

void _css_draw_pyramid(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct PyramidObj *obj = draw_obj->obj;
    if (!obj) return;
    int *vox = inner_info->vox;
    int width = draw_obj->basic.width;
    int height = draw_obj->basic.height;
    int depth = draw_obj->basic.depth;
    int w = width;
    int wh = width / 2;
    int dh = depth / 2;
    int h = height;
    int d = depth;

    struct WallObj *south_wall_obj = obj->south_wall;
    if (south_wall_obj && width > 0 && height > 0) {
        struct FlatImage* img_to_draw_first = _make_texture_from_wall(south_wall_obj, width, height);
        struct FlatImage* img_to_draw_second = _make_texture_from_wall(south_wall_obj, width, height);

        int voxes_first[9] = {
            vox[0] + 0 , vox[1] + 0 , vox[2] + 0,
            vox[0] + w , vox[1] + 0 , vox[2] + 0,
            vox[0] + wh, vox[1] + dh, vox[2] + h,
        };
        _css_draw_wide_triangle(inner_info->img, img_to_draw_first, voxes_first);

        int voxes_second[9] = {
            vox[0] + 0 , vox[1] + d , vox[2] + 0,
            vox[0] + w , vox[1] + d , vox[2] + 0,
            vox[0] + wh, vox[1] + dh, vox[2] + h,
        };
        _css_draw_wide_triangle(inner_info->img, img_to_draw_second, voxes_second);

        free(img_to_draw_first);
        free(img_to_draw_second);
    }

    struct WallObj *east_wall_obj = obj->east_wall;
    if (east_wall_obj && width > 0 && height > 0) {
        struct FlatImage* img_to_draw_first = _make_texture_from_wall(east_wall_obj, width, height);
        struct FlatImage* img_to_draw_second = _make_texture_from_wall(east_wall_obj, width, height);

        int voxes_first[9] = {
            vox[0] + 0 , vox[1] + 0 , vox[2] + 0,
            vox[0] + 0 , vox[1] + d , vox[2] + 0,
            vox[0] + wh, vox[1] + dh, vox[2] + h,
        };
        _css_draw_wide_triangle(inner_info->img, img_to_draw_first, voxes_first);

        int voxes_second[9] = {
            vox[0] + w , vox[1] + 0 , vox[2] + 0,
            vox[0] + w , vox[1] + d , vox[2] + 0,
            vox[0] + wh, vox[1] + dh, vox[2] + h,
        };
        _css_draw_wide_triangle(inner_info->img, img_to_draw_second, voxes_second);

        free(img_to_draw_first);
        free(img_to_draw_second);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] += width;
        out_vox[1] += depth;
        out_vox[2] += height;
    }
}

void _draw_component_in_series(
        struct SeriesObj *obj, struct DrawObj* draw_obj, 
        struct DrawInfo *draw_info, int *out_vox, char shift) {
    int inner_out_vox[3] = ZERO_VOX;
    int padding = obj->padding;
    enum FillDirection fill_direction = obj->fill_direction;

    draw_component(draw_obj, draw_info, inner_out_vox);

    if (shift) {
        draw_info->vox[fill_direction] += inner_out_vox[fill_direction] + padding;
    }

    if (out_vox) {
        if(inner_out_vox[0] > out_vox[0]) out_vox[0] = inner_out_vox[0];
        if(inner_out_vox[1] > out_vox[1]) out_vox[1] = inner_out_vox[1];
        if(inner_out_vox[2] > out_vox[2]) out_vox[2] = inner_out_vox[2];
    }
}

void _css_draw_series(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    struct SeriesObj *obj = draw_obj->obj;
    if (!obj) return;
    struct DrawObj **draw_objs = obj->objs;
    if (!draw_objs) return;

    int vox[3] = COPY_VOX(inner_info->vox);
    int *out_vox = inner_info->out_vox;
    struct DrawInfo draw_info = {
        .img=inner_info->img,
        .vox=vox,
    };

    struct DrawObj *left = obj->left;
    if (left) {
        _draw_component_in_series(obj, left, &draw_info, out_vox, 1);
    }

    struct DrawObj *right = obj->right;
    if (right) {
        _draw_component_in_series(obj, right, &draw_info, out_vox, 0);
    }

    struct DrawObj* middle = NULL;
    int index = 0;
    while((middle = draw_objs[index++])) {
        _draw_component_in_series(obj, middle, &draw_info, out_vox, 1);
    }

    if (out_vox) {
        enum FillDirection fill_direction = obj->fill_direction;
        out_vox[fill_direction] = inner_info->vox[fill_direction] - vox[fill_direction];
    }
}

void draw_component(struct DrawObj *draw_obj, struct DrawInfo *info, int *out_vox) {
    struct DrawInnerInfo inner_info = {
        .img=info->img,
        .vox=info->vox,
        .out_vox=out_vox,
    };

    switch(draw_obj->type) {
        case DRAW_OBJ_VOID: _css_draw_void(draw_obj, &inner_info); break;
        case DRAW_OBJ_CUBE: _css_draw_cube(draw_obj, &inner_info); break;
        case DRAW_OBJ_TRIANGLE: _css_draw_triangle(draw_obj, &inner_info); break;
        case DRAW_OBJ_PYRAMID: _css_draw_pyramid(draw_obj, &inner_info); break;
        case DRAW_OBJ_SERIES: _css_draw_series(draw_obj, &inner_info); break;
    }
}
