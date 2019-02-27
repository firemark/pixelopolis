#include <stdio.h>

#include "css_draw.h"
#include "css_func.h"
#include "draw_shape.h"
#include "img.h"

char* _get_floor_name(int index, int floors) {
    if (index == 0) return "bottom";
    else if (index == floors - 1) return "top";
    else return "middle"; 
}

int _get_max_height(struct Program* program, struct Rule* wall_rule) {
    int *height_ptr = css_find_number_prop(wall_rule, "height");
    if (height_ptr) {
        return *height_ptr;
    }
    // if height not found then compute total height from floors
    int max_height = 0;
    int *floors_ptr = css_find_number_prop(wall_rule, "floors");
    int floors = floors_ptr ? *floors_ptr : 1; 
    int i;

    for(i=0; i < floors; i++) {
        char* prop_floor_name = _get_floor_name(i, floors);
        struct Rule* floor_rule = css_find_rule_prop(program, wall_rule, prop_floor_name);
        if (!floor_rule) continue;
        int *height_ptr = css_find_number_prop(floor_rule, "height");
        max_height += height_ptr ? *height_ptr : 24;
    }

    return max_height;
}

void _css_draw_texture(
        struct image *img, 
        struct Rule* rule, 
        int vox[3],
        int width,
        int height,
        enum direction dir) {
    char* texture_url = css_find_string_prop(rule, "texture");
    if (!texture_url) return;
    struct image* texture = read_png_file(texture_url);
    width = width ? width : texture->width;
    height = height ? height : texture->height;
    draw_plane(img, texture, vox, width, height, dir);
    destroy_image(texture);
}

void _css_draw_wall_with_rule(
        struct image *img, 
        struct Program *program,
        struct Rule *wall_rule,
        int vox[3],
        enum direction dir) {
    int *width_ptr = css_find_number_prop(wall_rule, "width");
    int width = width_ptr ? *width_ptr : 50;
    int max_height = _get_max_height(program, wall_rule);

    int new_vox[3];
    new_vox[0] = vox[0];
    new_vox[1] = vox[1];
    new_vox[2] = vox[2];

    struct Rule* texture_rule = css_find_rule_prop(program, wall_rule, "texture");
    if (texture_rule) { // draw texture of wall
        _css_draw_texture(img, texture_rule, new_vox, width, max_height, dir);
    }

    int *floors_ptr = css_find_number_prop(wall_rule, "floors");
    int floors = floors_ptr ? *floors_ptr : 1; 
    int i;
    for(i=0; i < floors; i++) { // draw floors
        char* prop_floor_name = _get_floor_name(i, floors);
        struct Rule* floor_rule = css_find_rule_prop(program, wall_rule, prop_floor_name);
        if (!floor_rule) continue;
        int *height_ptr = css_find_number_prop(floor_rule, "height");
        int height = height_ptr ? *height_ptr : 24;

        struct Rule* floor_texture_rule = css_find_rule_prop(program, floor_rule, "texture");
        if (floor_texture_rule) {
            _css_draw_texture(img, floor_texture_rule, new_vox, width, height, dir);
            // TODO: Support list of textures
        }

        switch(dir) {
            case DIRECTION_UP: new_vox[1] += height; break;
            case DIRECTION_SOUTH: new_vox[2] += height; break;
            case DIRECTION_EAST: new_vox[2] += height; break;
        }

    }

}

void css_draw_wall(
        struct image *img, 
        struct Program *program,
        struct RuleSelector *query,
        int vox[3],
        enum direction dir) {
    struct Rule* wall_rule = css_find_rule_by_query(program, query);
    if (!wall_rule) return;

    _css_draw_wall_with_rule(img, program, wall_rule, vox, dir);
}

void css_draw_cube(
        struct image *img, 
        struct Program *program,
        struct RuleSelector *query,
        int vox[3]) {
    struct Rule* cube_rule = css_find_rule_by_query(program, query);
    if (!cube_rule) return;

    struct Rule* wall_rule = css_find_rule_prop(program, cube_rule, "wall");
    if (wall_rule) {
        _css_draw_wall_with_rule(img, program, wall_rule, vox, DIRECTION_EAST);
        _css_draw_wall_with_rule(img, program, wall_rule, vox, DIRECTION_SOUTH);
    }

    struct Rule* roof_rule = css_find_rule_prop(program, cube_rule, "roof");
    if (roof_rule) {
        _css_draw_wall_with_rule(img, program, roof_rule, vox, DIRECTION_UP);
    }
}
