#include <stdio.h>
#include <string.h>

#include "css_draw.h"
#include "css_func.h"
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
enum Shape { SHAPE_PLANE, SHAPE_TRIANGLE, SHAPE_PLANE_WITH_HEIGHT };

struct ShapeFunc {
    enum Shape shape;
    void *args;
};

struct ShapePlaneWithHeightArgs {
    int height;
};

char _get_index_height(enum direction dir) {
    switch(dir) {
        case DIRECTION_UP: return 1;
        default: return 2;
    }
}

char _get_index_width(enum direction dir) {
    switch(dir) {
        case DIRECTION_EAST: return 1;
        default: return 0;
    }
}

void _css_draw(struct DrawArgs *args, struct ShapeFunc *shape_func) {
    switch(shape_func->shape) {
        case SHAPE_PLANE: draw_plane(args); break;
        case SHAPE_TRIANGLE: draw_triangle(args); break;
        case SHAPE_PLANE_WITH_HEIGHT: {
            struct ShapePlaneWithHeightArgs *shape_args = shape_func->args;
            draw_plane_with_height(args, shape_args->height);
        } break;
    }
}

void _css_draw_texture(
        struct TexObj* obj,
        struct DrawInnerInfo* inner_info,
        int width,
        int height,
        enum direction dir,
        struct ShapeFunc *shape_func) {
    struct image* texture = obj->texture;
    if (!texture) return;
    width = width ? width : texture->width;
    height = height ? height : texture->height;

    struct DrawArgs args = {
        .img=inner_info->img,
        .img_to_draw=texture,
        .vox=inner_info->vox,
        .width=width,
        .height=height,
        .start_height=0,
        .max_height=height,
        .dir=dir,
    };

    _css_draw(&args, shape_func);
}

int _css_draw_floor(
        struct FloorObj* obj,
        struct DrawInnerInfo* inner_info,
        int width,
        int start_height,
        int limit_height,
        enum direction dir,
        enum Valign valign,
        struct ShapeFunc *shape_func) {
    struct image *img = inner_info->img;

    int *vox = inner_info->vox;
    int new_vox[3] = COPY_VOX(vox);
    int height = obj->height;
    char index_height = _get_index_height(dir);

    struct TexObj* tex_obj = obj->tex;
    if (!tex_obj) goto without_texture; 
    struct image* texture = tex_obj->texture;
    if (!texture) goto without_texture;

    width = width ? width : texture->width;
    height = height ? height : texture->height;

    if (valign == VALIGN_TOP) {
        start_height -= height;
    }

    struct DrawArgs args = {
        .img=inner_info->img,
        .img_to_draw=texture,
        .vox=new_vox,
        .width=width,
        .height=height,
        .start_height=start_height,
        .max_height=limit_height,
        .dir=dir,
    };

    _css_draw(&args, shape_func);

without_texture: ;
    char index_width = _get_index_width(dir);
    int start_width = 0;

    struct TexObj *left = obj->left;
    if (left && left->texture) {
        struct DrawArgs inner_args = {
            .img=inner_info->img,
            .img_to_draw=left->texture,
            .vox=new_vox,
            .width=left->texture->width,
            .height=left->texture->height,
            .start_height=start_height,
            .max_height=limit_height,
            .dir=dir,
        };
        _css_draw(&inner_args, shape_func);
        start_width += left->texture->width + obj->padding;
    }

    struct TexObj *right = obj->right;
    if (right && right->texture) {
        int right_vox[3] = COPY_VOX(new_vox);
        right_vox[index_width] += width - right->texture->width;
        struct DrawArgs inner_args = {
            .img=inner_info->img,
            .img_to_draw=right->texture,
            .vox=right_vox,
            .width=right->texture->width,
            .height=right->texture->height,
            .start_height=start_height,
            .max_height=limit_height,
            .dir=dir,
        };
        _css_draw(&inner_args, shape_func);
        draw_plane(&inner_args);
    }

    size_t obj_index;
    for(obj_index = 0; obj_index < obj->objs_size;  obj_index++) {
        struct TexObj *middle = obj->objs[obj_index];
        if (!middle || !middle->texture) {
            start_width += 12 + obj->padding;
            continue;
        }
        int middle_vox[3] = COPY_VOX(new_vox);
        middle_vox[index_width] += start_width;
        struct DrawArgs inner_args = {
            .img=inner_info->img,
            .img_to_draw=middle->texture,
            .vox=middle_vox,
            .width=middle->texture->width,
            .height=middle->texture->height,
            .start_height=start_height,
            .max_height=limit_height,
            .dir=dir,
        };
        _css_draw(&inner_args, shape_func);
        start_width += middle->texture->width + obj->padding;
    }

    return height;
}


void _css_draw_wall_with_rule(
        struct WallObj *obj,
        struct DrawInnerInfo* inner_info,
        int width,
        int height,
        enum direction dir,
        struct ShapeFunc* shape_func) {
    int *vox = inner_info->vox;
    struct image* img = inner_info->img;
    char index_height = _get_index_height(dir);

    struct TexObj *tex_obj = obj->tex;
    if (tex_obj) {
        struct DrawInnerInfo texture_inner_info = {
            .img=img,
            .vox=vox,
            .out_vox=NULL,
        };
        _css_draw_texture(tex_obj, &texture_inner_info, width, height, dir, shape_func);
    }

    int max_height = height;
    int start_height = 0;
    
    struct FloorObj *bottom = obj->bottom;
    if (bottom) {
        struct DrawInnerInfo texture_inner_info = {
            .img=img,
            .vox=vox,
            .out_vox=NULL,
        };
        int tex_height = _css_draw_floor(
            bottom, &texture_inner_info, width, start_height, max_height, dir, VALIGN_BOTTOM, shape_func);
        start_height += tex_height + obj->padding;
    }

    struct FloorObj *top = obj->top;
    if (top) {
        struct DrawInnerInfo texture_inner_info = {
            .img=img,
            .vox=vox,
            .out_vox=NULL,
        };
        int tex_height =_css_draw_floor(
                top, &texture_inner_info, width, max_height, max_height, dir, VALIGN_TOP, shape_func);
    }

    int floor_index;
    for(floor_index = 0; floor_index < obj->floors_size; floor_index++) {
        struct FloorObj *middle = obj->floors[floor_index];
        int tex_height = 0;
        if (middle) {
            struct DrawInnerInfo texture_inner_info = {
                .img=img,
                .vox=vox,
                .out_vox=NULL,
            };
            tex_height = _css_draw_floor(
                    middle, &texture_inner_info, width, start_height, max_height, dir, VALIGN_BOTTOM, shape_func);
        } else {
            tex_height = 12;
        }
        start_height += tex_height + obj->padding;
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] += width;
        out_vox[index_height] += height;
    }
}

void _css_draw_cube(struct CubeObj *obj, struct DrawInnerInfo *inner_info) {
    if (!obj) return;
    int *vox = inner_info->vox;
    int width = obj->basic.width;
    int height = obj->basic.height;
    int depth = obj->basic.depth;

    struct ShapeFunc shape_func = {
        .shape=SHAPE_PLANE,
        .args=NULL,
    };

    struct WallObj *east_wall_obj = obj->east_wall;
    if (east_wall_obj) {
        int east_vox[3] = {vox[0] + width, vox[1], vox[2]};
        struct DrawInnerInfo wall_info = {
            .img=inner_info->img,
            .vox=east_vox,
        };
        _css_draw_wall_with_rule(east_wall_obj, &wall_info, depth, height, DIRECTION_EAST, &shape_func);
    }

    struct WallObj *south_wall_obj = obj->south_wall;
    if (south_wall_obj) {
        int south_vox[3] = {vox[0], vox[1], vox[2]};
        struct DrawInnerInfo wall_info = {
            .img=inner_info->img,
            .vox=south_vox,
        };
        _css_draw_wall_with_rule(south_wall_obj, &wall_info, width, height, DIRECTION_SOUTH, &shape_func);
    }

    struct WallObj *roof_obj = obj->roof;
    if (roof_obj) {
        int up_vox[3] = {vox[0], vox[1], vox[2] + height};
        struct DrawInnerInfo roof_info = {
            .img=inner_info->img,
            .vox=up_vox,
        };
        _css_draw_wall_with_rule(roof_obj, &roof_info, width, depth, DIRECTION_UP, &shape_func);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] += width;
        out_vox[1] += depth;
        out_vox[2] += height;
    }
}

void _css_draw_triangle(struct TriangleObj *obj, struct DrawInnerInfo *inner_info) {
    if (!obj) return;
    int *vox = inner_info->vox;
    int width = obj->basic.width;
    int height = obj->basic.height;
    int depth = obj->basic.depth;

    struct DrawInnerInfo wall_info = {
        .img=inner_info->img,
        .vox=vox,
    };

    struct WallObj *wall_obj = obj->wall;
    if (wall_obj) {
        struct ShapeFunc shape_func = {
            .shape=SHAPE_TRIANGLE,
            .args=NULL,
        };
        _css_draw_wall_with_rule(wall_obj, &wall_info, width, height, DIRECTION_SOUTH, &shape_func);
    }

    struct WallObj *roof_obj = obj->roof;
    if (roof_obj) {
        struct ShapePlaneWithHeightArgs args = {
            .height=height,
        };
        struct ShapeFunc shape_func = {
            .shape=SHAPE_PLANE_WITH_HEIGHT,
            .args=&args,
        };
        _css_draw_wall_with_rule(roof_obj, &wall_info, width, depth, DIRECTION_EAST, &shape_func);
    }

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
