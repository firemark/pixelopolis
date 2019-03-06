#include <stdio.h>

#include "css_draw.h"
#include "css_func.h"
#include "draw_shape.h"
#include "img.h"

struct DrawInnerInfo {
    struct image *img; 
    struct Program *program;
    struct Rule *self;
    struct Rule *parent;
    int *vox;
};

enum Valign { VALIGN_TOP, VALIGN_BOTTOM };
enum Series { VERTICAL_SERIES, HORIZONTAL_SERIES };

char _get_index_height(enum direction dir) {
    switch(dir) {
        case DIRECTION_UP: return 1;
        default: return 2;
    }
}

void _css_draw_texture(
        struct DrawInnerInfo* inner_info,
        int width,
        int height,
        enum direction dir) {
    struct Program *program = inner_info->program;
    struct image *img = inner_info->img;
    int *vox = inner_info->vox;

    char* texture_url = css_find_string_prop(inner_info->self, "texture");
    if (!texture_url) return;

    struct image* texture = read_png_file(texture_url);
    width = width ? width : texture->width;
    height = height ? height : texture->height;

    draw_plane(img, texture, vox, width, height, dir);
    destroy_image(texture);
}

int _css_draw_floor(
        struct DrawInnerInfo* inner_info,
        int width,
        int limit_height,
        enum direction dir,
        enum Valign valign) {
    struct Program *program = inner_info->program;
    struct image *img = inner_info->img;
    struct Rule *floor_rule = inner_info->self;

    int *vox = inner_info->vox;
    int new_vox[3] = {vox[0], vox[1], vox[2]};
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
    struct image* texture = read_png_file(texture_url);

    width = width ? width : texture->width;
    height = height ? height : texture->height;

    if (valign == VALIGN_TOP) {
        new_vox[index_height] -= height;
    }

    if (limit_height > 0 && limit_height < vox[index_height] + height)  {
        destroy_image(texture);
        return 0;
    }
    draw_plane(img, texture, new_vox, width, height, dir);
    destroy_image(texture);
    return height;

    without_texture:
    if (valign == VALIGN_TOP) {
        new_vox[index_height] -= height;
    }
    if (limit_height > 0 && limit_height < vox[index_height] + height)  {
        return 0;
    }
    return height;
}

char* _get_width_name_for_wall(enum direction dir) {
    switch(dir) {
        case DIRECTION_EAST: return "depth"; 
        default: return "width";
    }
}

char* _get_height_name_for_wall(enum direction dir) {
    switch(dir) {
        case DIRECTION_UP: return "depth"; 
        default: return "height";
    }
}

void _css_draw_wall_with_rule(
        struct DrawInnerInfo* inner_info,
        enum direction dir) {
    struct Program *program = inner_info->program;
    struct image *img = inner_info->img;
    struct Rule *wall_rule = inner_info->self;
    struct Rule *parent_rule = inner_info->parent;
    int *vox = inner_info->vox;

    char* width_name =_get_width_name_for_wall(dir);
    char* height_name = _get_height_name_for_wall(dir);
    int *width_ptr = css_find_number_prop(parent_rule, width_name); 
    int *height_ptr = css_find_number_prop(parent_rule, height_name); 
    int width = width_ptr ? *width_ptr : 50;
    int height = height_ptr ? *height_ptr : 50;
    char index_height = _get_index_height(dir);

    struct Rule* texture_rule = css_find_rule_prop(program, wall_rule, "texture");
    if (texture_rule) {
        struct DrawInnerInfo texture_inner_info = {
            .program=program,
            .img=img,
            .self=texture_rule,
            .parent=wall_rule,
            .vox=vox,
        };
        _css_draw_texture(&texture_inner_info, width, height, dir);
    }

    int max_height = height + vox[index_height];
    int loop_vox[3] = {vox[0], vox[1], vox[2]};
    
    struct Rule* bottom_rule = css_find_rule_prop(program, wall_rule, "bottom");
    if (bottom_rule) {
        struct DrawInnerInfo texture_inner_info = {
            .program=program,
            .img=img,
            .self=bottom_rule,
            .parent=wall_rule,
            .vox=loop_vox,
        };
        int bottom_height = _css_draw_floor(&texture_inner_info, width, 0, dir, VALIGN_BOTTOM);
        loop_vox[index_height] += bottom_height;
    }

    struct Rule* top_rule = css_find_rule_prop(program, wall_rule, "top");
    if (top_rule) {
        int new_vox[3] = {vox[0], vox[1], vox[2]};
        new_vox[index_height] += height;
        struct DrawInnerInfo texture_inner_info = {
            .program=program,
            .img=img,
            .self=top_rule,
            .parent=wall_rule,
            .vox=new_vox,
        };
        int top_height = _css_draw_floor(&texture_inner_info, width, 0, dir, VALIGN_TOP);
        max_height -= top_height;
    }

    struct Rule* middle_rule = css_find_rule_prop(program, wall_rule, "middle");
    if (!middle_rule) {
        return;
    }

    struct DrawInnerInfo texture_inner_info = {
        .program=program,
        .img=img,
        .self=middle_rule,
        .parent=wall_rule,
        .vox=loop_vox,
    };

    for(;;) {
        int floor_height = _css_draw_floor(&texture_inner_info, width, max_height, dir, VALIGN_BOTTOM);
        if (floor_height == 0) break;
        loop_vox[index_height] += floor_height;
    }

}

void _css_draw_cube(struct DrawInnerInfo *inner_info) {
    struct Program* program = inner_info->program;
    struct Rule* cube_rule = inner_info->self;
    int *vox = inner_info->vox;

    int *width_ptr = css_find_number_prop(cube_rule, "width"); 
    int *height_ptr = css_find_number_prop(cube_rule, "height"); 
    int width = width_ptr ? *width_ptr : 50;
    int height = height_ptr ? *height_ptr : 50;

    struct Rule* wall_rule = css_find_rule_prop(program, cube_rule, "wall");
    if (wall_rule) {
        struct DrawInnerInfo wall_inner_info = {
            .program=program,
            .img=inner_info->img,
            .self=wall_rule,
            .parent=cube_rule,
            .vox=NULL,
        };
        int east_vox[3] = {vox[0] + width, vox[1], vox[2]};
        int south_vox[3] = {vox[0], vox[1], vox[2]};
        wall_inner_info.vox = east_vox;
        _css_draw_wall_with_rule(&wall_inner_info, DIRECTION_EAST);
        wall_inner_info.vox = south_vox;
        _css_draw_wall_with_rule(&wall_inner_info, DIRECTION_SOUTH);
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
        };
        _css_draw_wall_with_rule(&roof_inner_info, DIRECTION_UP);
    }
}

void _css_draw_series(struct DrawInnerInfo &inner_info, enum Series) {
    struct Program *program = inner_info->program;
    struct Rule* series = inner_info->self;
    if (!series) return;
    struct Prop* buildings_prop = css_find_prop(series, "buildings");
    if (!buildings_prop) return;

    int *padding_ptr = css_find_number_prop(series, "padding");
    int padding = padding_ptr ? *padding_ptr : 8;

    struct Obj* obj = NULL;

    css_iter(obj, series_prop->objs) {
        if (obj->type != OBJ_RULE) continue;
        struct RuleSelector *building_query = obj->value;
        struct DrawInfo draw_info = {
            .img=img,
            .program=program,
            .query=building_query,
            .vox=vox,
        };
        draw_component(&draw_info);
        // todo: add output attributes after draw
    }
}

int _check_element_name(struct RuleSelector *selector, char* name) {
    return strcmp(selector->element, name) == 0;
}

void draw_component(struct DrawInfo *info) {
    struct RuleSelector *query = info->query;
    struct Rule* rule = css_find_rule_by_query(info->program, query);
    if (!rule) return;

    struct DrawInnerInfo inner_info = {
        .program=info->program,
        .img=info->img,
        .self=rule,
        .parent=NULL,
        .vox=info->vox,
    };

    if (_check_element_name(query, "cube")) {
        _css_draw_cube(&inner_info);
    } else if (_check_element_name(query, "pyramid")) {
        // _css_draw_pyramid(&inner_info);
    } else if (_check_element_name(query, "v-series")) {
        _css_draw_series(&inner_info, VERTICAL_SERIES);
    } else if (_check_element_name(query, "h-series")) {
        _css_draw_series(&inner_info, HORIZONTAL_SERIES);
    } else if (_check_element_name(query, "stack")) {
        // _css_draw_stack(&inner_info);
    }

}
