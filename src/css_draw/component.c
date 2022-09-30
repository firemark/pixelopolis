#include "pixelopolis/_css_draw.h"

void draw_component(struct DrawObj *draw_obj, struct DrawInfo *info, int *out_vox) {
    struct DrawInnerInfo inner_info = {
        .img = info->img,
        .vox = info->vox,
        .out_vox = out_vox,
        .filter = info->filter,
    };

    switch (draw_obj->type) {
        case DRAW_OBJ_VOID:
            css_draw_void(draw_obj, &inner_info);
            break;
        case DRAW_OBJ_CUBE:
            css_draw_cube(draw_obj, &inner_info);
            break;
        case DRAW_OBJ_CONE:
            css_draw_cone(draw_obj, &inner_info);
            break;
        case DRAW_OBJ_CYLINDER:
            css_draw_cylinder(draw_obj, &inner_info);
            break;
        case DRAW_OBJ_DOME:
            css_draw_dome(draw_obj, &inner_info);
            break;
        case DRAW_OBJ_TRIANGLE:
            css_draw_triangle(draw_obj, &inner_info);
            break;
        case DRAW_OBJ_PYRAMID:
            css_draw_pyramid(draw_obj, &inner_info);
            break;
        case DRAW_OBJ_SERIES:
            css_draw_series(draw_obj, &inner_info);
            break;
        case DRAW_OBJ_BOARD:
            css_draw_board(draw_obj, &inner_info);
            break;
    }
}
