#include <stdlib.h>

#include "pixelopolis/_draw_builder.h"
#include "pixelopolis/css_func.h"
// #include "pixelopolis/draw_builder/"

// void _append_floors_to_wall(struct Helper* helper, struct WallObj* wall, int wall_width,
//                             int wall_height) {
//     int y = 0;
//     int size = 0;
//     int padding = wall->padding;
//     struct FloorObj** floors = malloc(sizeof(struct FloorObj*) * BUILDER_MAX_ELEMENTS);
//     struct Rule* rule = helper->rule;
//     struct Program* program = helper->program;

//     struct FloorObj* bottom = NULL;
//     struct FloorObj* top = NULL;

//     struct SelectorHelper bottom_helper = {
//         .program = program,
//         .parent_rule = rule,
//         .selector = css_find_selector_prop(rule->rule, "bottom"),
//         .parent = helper->parent,
//     };
//     bottom = builder_build_floor(&bottom_helper, wall_width);
//     if (bottom) {
//         if (bottom->height < wall_width) {
//             y += bottom->height + padding;
//         } else {
//             bottom = NULL;
//             top = NULL;
//             goto final;
//         }
//     }

//     struct SelectorHelper top_helper = {
//         .program = program,
//         .parent_rule = rule,
//         .selector = css_find_selector_prop(rule->rule, "top"),
//         .parent = helper->parent,
//     };
//     top = builder_build_floor(&top_helper, wall_width);
//     if (top) {
//         if (y + top->height < wall_height) {
//             wall_height -= top->height;
//         } else {
//             top = NULL;
//             goto final;
//         }
//     }

//     for (;;) {
//         struct SelectorHelper middle_helper = {
//             .program = program,
//             .parent_rule = rule,
//             .selector = css_find_selector_prop(rule->rule, "middle"),
//             .parent = helper->parent,
//         };
//         struct FloorObj* middle = builder_build_floor(&middle_helper, wall_width);
//         int middle_height = middle ? middle->height : 12;
//         if (y + middle_height >= wall_height) break;
//         y += middle_height + padding;
//         floors[size++] = middle;
//     }

// final:
//     floors[size] = NULL;
//     wall->floors = floors;
//     wall->floors_size = size;
//     wall->bottom = bottom;
//     wall->top = top;
// }

struct WallObj* builder_build_wall(struct SelectorHelper* helper, int wall_width, int wall_height) {
    struct RuleWithParent* rule = builder_make_rule_from_helper(helper);
    if (!rule) return NULL;
    struct WallObj* wall = malloc(sizeof(struct WallObj));

    wall->padding = builder_get_padding(rule);
    wall->points_density = builder_get_percent(rule, "points-density", 0);
    wall->width = wall_width;
    wall->height = wall_height;

    struct SelectorHelper tex_helper = {
        .program = helper->program,
        .parent_rule = rule,
        .selector = css_find_selector_prop(rule->rule, "texture"),
        .parent = helper->parent,
    };
    wall->tex = builder_make_texture(&tex_helper, wall_width, wall_height);

    struct SelectorHelper points_tex_helper = {
        .program = helper->program,
        .parent_rule = rule,
        .selector = css_find_selector_prop(rule->rule, "points-texture"),
        .parent = helper->parent,
    };
    wall->points_tex = builder_make_texture(&points_tex_helper, 8, 8);

    // struct Helper inner_helper = {
    //     .program = helper->program,
    //     .rule = rule,
    //     .parent = helper->parent,
    // };
    // _append_floors_to_wall(&inner_helper, wall, wall_width, wall_height);
    return wall;
}
