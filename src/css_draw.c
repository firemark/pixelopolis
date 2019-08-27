#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "css_draw.h"
#include "css_func.h"
#include "draw_poly.h"
#include "draw_shape.h"
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
    if (valign == VALIGN_BOTTOM) {
        height += _get_height_of_floor(obj);
    }

    struct TexObj* tex_obj = obj->tex;
    if (!tex_obj) goto without_texture; 
    struct FlatImage* texture = tex_obj->texture;
    if (!texture) goto without_texture;
    flat_image_fill_column(img, texture, height);

without_texture:;
    int start_width = 0;
    struct TexObj *left = obj->left;
    if (left && left->texture) {
        flat_image_copy(img, left->texture, height, start_width);
        start_width += left->texture->width + obj->padding;
    }

    struct TexObj *right = obj->right;
    if (right && right->texture) {
        flat_image_copy(img, right->texture, img->width - right->texture->width, height);
    }

    size_t obj_index;
    for(obj_index = 0; obj_index < obj->objs_size; obj_index++) {
        struct TexObj *middle = obj->objs[obj_index];
        if (!middle || !middle->texture) {
            start_width += 12 + obj->padding;
            continue;
        }
        flat_image_copy(img, middle->texture, start_width, height);
        start_width += middle->texture->width + obj->padding;
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
        _draw_floor_on_texture(img, top, start_height, VALIGN_TOP);
    }

    int floor_index;
    for(floor_index = 0; floor_index < obj->floors_size; floor_index++) {
        struct FloorObj *middle = obj->floors[floor_index];
        int tex_height = 0;
        if (middle) {
            _draw_floor_on_texture(img, middle, start_height, VALIGN_BOTTOM);
            tex_height = _get_height_of_floor(bottom);
        } else {
            tex_height = 12;
        }
        start_height += tex_height + obj->padding;
    }

    return img;
}

void _css_draw_plane(struct image* img, struct FlatImage* img_to_draw, int voxes[12]) {
    /* 3---4
     * |B /|
     * | / |
     * |/ A|
     * 1---2 
     */
    int voxes_a[9] = {
        voxes[0], voxes[1], voxes[2], // 1
        voxes[3], voxes[4], voxes[5], // 2
        voxes[9], voxes[10], voxes[11] // 4
    };
    int uv_a[6] = {
        0, 0,
        img_to_draw->width, 0,
        0, img_to_draw->height
    };

    int voxes_b[9] = {
        voxes[0], voxes[1], voxes[2], // 1
        voxes[6], voxes[7], voxes[8], // 3
        voxes[9], voxes[10], voxes[11] // 4
    };
    int uv_b[6] = {
        0, 0,
        0, img_to_draw->height,
        img_to_draw->width, img_to_draw->height
    };

    draw_poly(img, img_to_draw, voxes_a, uv_a);
    draw_poly(img, img_to_draw, voxes_b, uv_b);
}

void _css_draw_cube(struct CubeObj *obj, struct DrawInnerInfo *inner_info) {
    if (!obj) return;
    int *vox = inner_info->vox;
    int width = obj->basic.width;
    int height = obj->basic.height;
    int depth = obj->basic.depth;

    struct WallObj *east_wall_obj = obj->east_wall;
    if (east_wall_obj && depth > 0 && height > 0) {
        int voxes[12] = {
            vox[0] + width, vox[1], vox[2],
            vox[0] + width, vox[1] + depth, vox[2],
            vox[0] + width, vox[1], vox[2] + height,
            vox[0] + width, vox[1] + depth, vox[2] + height,
        };
        struct FlatImage* img_to_draw = _make_texture_from_wall(east_wall_obj, depth, height);
        _css_draw_plane(inner_info->img, img_to_draw, voxes);
        free(img_to_draw);
    }

    struct WallObj *south_wall_obj = obj->south_wall;
    if (south_wall_obj && width > 0 && height > 0) {
        int voxes[12] = {
            vox[0], vox[1], vox[2],
            vox[0] + width, vox[1], vox[2],
            vox[0], vox[1], vox[2] + height,
            vox[0] + width, vox[1], vox[2] + height
        };
        struct FlatImage* img_to_draw = _make_texture_from_wall(south_wall_obj, width, height);
        _css_draw_plane(inner_info->img, img_to_draw, voxes);
        free(img_to_draw);
    }

    struct WallObj *roof_obj = obj->roof;
    if (roof_obj && width > 0 && depth > 0) {
        int voxes[12] = {
            vox[0], vox[1], vox[2] + height,
            vox[0] + width, vox[1], vox[2] + height,
            vox[0], vox[1] + depth, vox[2] + height,
            vox[0] + width, vox[1] + depth, vox[2] + height
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
    /*     3
     *    / \
     *   /   \
     *  /     \
     * 1-------2 
     */
    int uv[6] = {
        0, 0,
        img_to_draw->width, 0,
        img_to_draw->width / 2, img_to_draw->height
    }; 
    draw_poly(img, img_to_draw, voxes, uv);
}

void _css_draw_triangle(struct TriangleObj *obj, struct DrawInnerInfo *inner_info) {
    if (!obj) return;
    int *vox = inner_info->vox;
    int width = obj->basic.width;
    int height = obj->basic.height;
    int depth = obj->basic.depth;

    struct WallObj *wall_obj = obj->wall;
    if (wall_obj && width > 0 && height > 0) {
        int voxes[9] = {
            vox[0], vox[1], vox[2],
            vox[0] + width, vox[1], vox[2],
            vox[0] + width / 2, vox[1], vox[2] + height,
        };
        struct FlatImage* img_to_draw = _make_texture_from_wall(wall_obj, width, height);
        _css_draw_wide_triangle(inner_info->img, img_to_draw, voxes);
        free(img_to_draw);
    }

//    struct WallObj *roof_obj = obj->roof;
//    if (roof_obj) {
//        int voxes[12] = {
//        };
//        struct FlatImage* img_to_draw = _make_texture_from_wall(roof_obj, width, height);
//        _css_draw_plane(inner_info->img, img_to_draw, voxes);
//        free(img_to_draw);
//    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] += width;
        out_vox[1] += depth;
        out_vox[2] += height;
    }
}

void _css_draw_series(struct SeriesObj *obj, struct DrawInnerInfo *inner_info) {
    if (!obj) return;
    struct DrawObj **draw_objs = obj->objs;
    if (!draw_objs) return;

    int padding = obj->padding;
    int series_index = obj->series;

    int vox[3] = COPY_VOX(inner_info->vox);
    int *out_vox = inner_info->out_vox;

    struct DrawObj* draw_obj = NULL;
    int index = 0;
    while(draw_obj = draw_objs[index++]) {
        int inner_out_vox[3] = ZERO_VOX;
        struct RuleSelector *building_query = draw_obj->obj;
        struct DrawInfo draw_info = {
            .img=inner_info->img,
            .vox=vox,
        };
        draw_component(draw_obj, &draw_info, inner_out_vox);
        vox[series_index] += inner_out_vox[series_index] + padding;
        if (out_vox) {
            if(inner_out_vox[0] > out_vox[0]) out_vox[0] = inner_out_vox[0];
            if(inner_out_vox[1] > out_vox[1]) out_vox[1] = inner_out_vox[1];
            if(inner_out_vox[2] > out_vox[2]) out_vox[2] = inner_out_vox[2];
        }
    }

    if (out_vox) {
        out_vox[series_index] = inner_info->vox[series_index] - vox[series_index];
    }
}

void draw_component(struct DrawObj *draw_obj, struct DrawInfo *info, int *out_vox) {
    struct DrawInnerInfo inner_info = {
        .img=info->img,
        .vox=info->vox,
        .out_vox=out_vox,
    };

    switch(draw_obj->type) {
        case DRAW_OBJ_CUBE: _css_draw_cube(draw_obj->obj, &inner_info); break;
        case DRAW_OBJ_TRIANGLE: _css_draw_triangle(draw_obj->obj, &inner_info); break;
        case DRAW_OBJ_SERIES: _css_draw_series(draw_obj->obj, &inner_info); break;
    }
}
