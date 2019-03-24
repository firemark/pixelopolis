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
    struct Program *program;
    struct Rule *self;
    struct Rule *parent;
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

struct HashMap* textures;

void css_init(void) {
    textures = hash_make();
}

void css_stop(void) {
    struct image* texture;
    hash_iter_values(texture, textures) {
        destroy_image(texture);
    }
    hash_destroy(textures);
}

struct image* _get_texture(char *filename) {
    struct image* texture;

    texture = hash_get(textures, filename);
    if (texture) {
        return texture;
    }

    texture = read_png_file(filename);
    if (!texture) {
        return NULL;
    }

    hash_set(textures, filename, texture, NULL);
    return texture;
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
        struct DrawInnerInfo* inner_info,
        int width,
        int height,
        enum direction dir,
        struct ShapeFunc *shape_func) {
    struct Program *program = inner_info->program;

    char* texture_url = css_find_string_prop(inner_info->self, "texture");
    if (!texture_url) return;

    struct image* texture = _get_texture(texture_url);
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
        struct DrawInnerInfo* inner_info,
        int width,
        int start_height,
        int limit_height,
        enum direction dir,
        enum Valign valign,
        struct ShapeFunc *shape_func) {
    struct Program *program = inner_info->program;
    struct image *img = inner_info->img;
    struct Rule *floor_rule = inner_info->self;

    int *vox = inner_info->vox;
    int new_vox[3] = COPY_VOX(vox);
    int height = 0;
    char index_height = _get_index_height(dir);

    int *height_ptr = css_find_number_prop(floor_rule, "height");
    if (height_ptr) {
        height = *height_ptr;
    }

    struct Rule* texture_rule = css_find_rule_prop(program, floor_rule, "texture");

    if (!texture_rule) goto without_texture; 
    char* texture_url = css_find_string_prop(texture_rule, "texture");
    if (!texture_url) goto without_texture;
    struct image* texture = _get_texture(texture_url);
    if (!texture) goto without_texture;

    width = width ? width : texture->width;
    height = height ? height : texture->height;

    if (valign == VALIGN_TOP) {
        new_vox[index_height] -= height;
        start_height -= height;
    }

    if (limit_height < start_height + height)  {
        return 0;
    }

    struct DrawArgs args = {
        .img=inner_info->img,
        .img_to_draw=texture,
        .vox=inner_info->vox,
        .width=width,
        .height=height,
        .start_height=start_height,
        .max_height=limit_height,
        .dir=dir,
    };

    _css_draw(&args, shape_func);
    return height;

    without_texture:
    if (limit_height < start_height + height) {
        return 0;
    }

    return height;
}


void _css_draw_wall_with_rule(
        struct DrawInnerInfo* inner_info,
        int width,
        int height,
        enum direction dir,
        struct ShapeFunc* shape_func) {
    struct Program *program = inner_info->program;
    struct image *img = inner_info->img;
    struct Rule *wall_rule = inner_info->self;
    struct Rule *parent_rule = inner_info->parent;
    int *vox = inner_info->vox;
    char index_height = _get_index_height(dir);

    struct Rule* texture_rule = css_find_rule_prop(program, wall_rule, "texture");
    if (texture_rule) {
        struct DrawInnerInfo texture_inner_info = {
            .program=program,
            .img=img,
            .self=texture_rule,
            .parent=wall_rule,
            .vox=vox,
            .out_vox=NULL,
        };
        _css_draw_texture(&texture_inner_info, width, height, dir, shape_func);
    }

    int max_height = height;
    int start_height = 0;
    
    struct Rule* bottom_rule = css_find_rule_prop(program, wall_rule, "bottom");
    if (bottom_rule) {
        int out_vox[3] = ZERO_VOX;
        struct DrawInnerInfo texture_inner_info = {
            .program=program,
            .img=img,
            .self=bottom_rule,
            .parent=wall_rule,
            .vox=vox,
            .out_vox=NULL,
        };
        int tex_height = _css_draw_floor(
            &texture_inner_info, width, start_height, max_height, dir, VALIGN_BOTTOM, shape_func);
        start_height += tex_height;
    }

    struct Rule* top_rule = css_find_rule_prop(program, wall_rule, "top");
    if (top_rule) {
        int top_vox[3] = COPY_VOX(vox);
        int out_vox[3] = ZERO_VOX;
        top_vox[index_height] += height;
        struct DrawInnerInfo texture_inner_info = {
            .program=program,
            .img=img,
            .self=top_rule,
            .parent=wall_rule,
            .vox=top_vox,
            .out_vox=NULL,
        };
        int tex_height =_css_draw_floor(
                &texture_inner_info, width, max_height, max_height, dir, VALIGN_TOP, shape_func);
        max_height -= tex_height;
    }

    struct Rule* middle_rule = css_find_rule_prop(program, wall_rule, "middle");
    if (middle_rule) {
        struct DrawInnerInfo texture_inner_info = {
            .program=program,
            .img=img,
            .self=middle_rule,
            .parent=wall_rule,
            .vox=vox,
            .out_vox=NULL,
        };

        for(;;) {
            int tex_height = _css_draw_floor(
                    &texture_inner_info, width, start_height, max_height, dir, VALIGN_BOTTOM, shape_func);
            if (tex_height == 0) break;
            start_height += tex_height;
        }
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] += width;
        out_vox[index_height] += height;
    }
}

void _css_draw_cube(struct DrawInnerInfo *inner_info) {
    struct Program* program = inner_info->program;
    struct Rule* cube_rule = inner_info->self;
    int *vox = inner_info->vox;

    int *width_ptr = css_find_number_prop(cube_rule, "width"); 
    int *height_ptr = css_find_number_prop(cube_rule, "height"); 
    int *depth_ptr = css_find_number_prop(cube_rule, "depth"); 
    int width = width_ptr ? *width_ptr : 50;
    int height = height_ptr ? *height_ptr : 50;
    int depth = depth_ptr ? *depth_ptr : 50;

    struct Rule* wall_rule = css_find_rule_prop(program, cube_rule, "wall");
    struct ShapeFunc shape_func = {
        .shape=SHAPE_PLANE,
        .args=NULL,
    };
    if (wall_rule) {
        struct DrawInnerInfo wall_inner_info = {
            .program=program,
            .img=inner_info->img,
            .self=wall_rule,
            .parent=cube_rule,
            .vox=NULL,
            .out_vox=NULL,
        };
        int east_vox[3] = {vox[0] + width, vox[1], vox[2]};
        int south_vox[3] = {vox[0], vox[1], vox[2]};
        wall_inner_info.vox = east_vox;
        _css_draw_wall_with_rule(&wall_inner_info, depth, height, DIRECTION_EAST, &shape_func);
        wall_inner_info.vox = south_vox;
        _css_draw_wall_with_rule(&wall_inner_info, width, height, DIRECTION_SOUTH, &shape_func);
    }

    struct Rule* roof_rule = css_find_rule_prop(program, cube_rule, "roof");
    if (roof_rule) {
        int up_vox[3] = {vox[0], vox[1], vox[2] + height};
        struct DrawInnerInfo roof_inner_info = {
            .program=program,
            .img=inner_info->img,
            .self=roof_rule,
            .parent=cube_rule,
            .vox=up_vox,
            .out_vox=NULL,
        };
        _css_draw_wall_with_rule(&roof_inner_info, width, depth, DIRECTION_UP, &shape_func);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] += width;
        out_vox[1] += depth;
        out_vox[2] += height;
    }
}

void _css_draw_triangle(struct DrawInnerInfo *inner_info) {
    struct Program* program = inner_info->program;
    struct Rule* rule = inner_info->self;
    int *vox = inner_info->vox;

    int *width_ptr = css_find_number_prop(rule, "width"); 
    int *height_ptr = css_find_number_prop(rule, "height"); 
    int *depth_ptr = css_find_number_prop(rule, "depth"); 
    int width = width_ptr ? *width_ptr : 50;
    int height = height_ptr ? *height_ptr : 50;
    int depth = depth_ptr ? *depth_ptr : 50;

    struct Rule* wall_rule = css_find_rule_prop(program, rule, "wall");
    if (wall_rule) {
        struct DrawInnerInfo wall_inner_info = {
            .program=program,
            .img=inner_info->img,
            .self=wall_rule,
            .parent=rule,
            .vox=vox,
            .out_vox=NULL,
        };
        struct ShapeFunc shape_func = {
            .shape=SHAPE_TRIANGLE,
            .args=NULL,
        };
        _css_draw_wall_with_rule(&wall_inner_info, width, height, DIRECTION_SOUTH, &shape_func);
    }

    struct Rule* roof_rule = css_find_rule_prop(program, rule, "roof");
    if (roof_rule) {
        struct DrawInnerInfo roof_inner_info = {
            .program=program,
            .img=inner_info->img,
            .self=roof_rule,
            .parent=rule,
            .vox=vox,
            .out_vox=NULL,
        };
        struct ShapePlaneWithHeightArgs args = {
            .height=height,
        };
        struct ShapeFunc shape_func = {
            .shape=SHAPE_PLANE_WITH_HEIGHT,
            .args=&args,
        };
        _css_draw_wall_with_rule(&roof_inner_info, width, depth, DIRECTION_EAST, &shape_func);
    }

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] += width;
        out_vox[1] += depth;
        out_vox[2] += height;
    }
}

void _css_draw_series(struct DrawInnerInfo *inner_info, enum Series series_index) {
    struct Rule* series = inner_info->self;
    if (!series) return;
    struct Prop* body_prop = css_find_prop(series, "body");
    if (!body_prop) return;

    int *padding_ptr = css_find_number_prop(series, "padding");
    int padding = padding_ptr ? *padding_ptr : 0;

    struct Obj* obj = NULL;
    int vox[3] = COPY_VOX(inner_info->vox);
    int *out_vox = inner_info->out_vox;

    css_iter(obj, body_prop->objs) {
        if (obj->type != OBJ_RULE) continue;
        int inner_out_vox[3] = ZERO_VOX;
        struct RuleSelector *building_query = obj->value;
        struct DrawInfo draw_info = {
            .img=inner_info->img,
            .program=inner_info->program,
            .query=building_query,
            .vox=vox,
        };
        draw_component(&draw_info, inner_out_vox);
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

int _check_element_name(struct RuleSelector *selector, char *name) {
    return strcmp(selector->element, name) == 0;
}

void draw_component(struct DrawInfo *info, int *out_vox) {
    struct RuleSelector *query = info->query;
    struct Rule* rule = css_find_rule_by_query(info->program, query);
    if (!rule) return;

    struct DrawInnerInfo inner_info = {
        .program=info->program,
        .img=info->img,
        .self=rule,
        .parent=NULL,
        .vox=info->vox,
        .out_vox=out_vox,
    };

    if (_check_element_name(query, "cube")) {
        _css_draw_cube(&inner_info);
    } else if (_check_element_name(query, "triangle")) {
        _css_draw_triangle(&inner_info);
    } else if (_check_element_name(query, "pyramid")) {
        // _css_draw_pyramid(&inner_info);
    } else if (_check_element_name(query, "v-series")) {
        _css_draw_series(&inner_info, VERTICAL_SERIES);
    } else if (_check_element_name(query, "h-series")) {
        _css_draw_series(&inner_info, HORIZONTAL_SERIES);
    } else if (_check_element_name(query, "d-series")) {
        _css_draw_series(&inner_info, DEPTH_SERIES);
    }
}
