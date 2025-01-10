#pragma once
#include <math.h>

#include "pixelopolis/draw_builder_world.h"
#include "pixelopolis/hash.h"

#include "pixelopolis/_draw_builder_utils.h"

#define BUILDER_MAX_ELEMENTS 128

struct Helper {
    struct Program* program;
    struct RuleWithParent* rule;
    struct DrawObj* parent;
};

struct SelectorHelper {
    struct Program* program;
    struct RuleWithParent* parent_rule;
    struct RuleSelector* selector;
    struct DrawObj* parent;
};

#define HELPER_ALLOCATE(helper, type) MEMORY_ALLOCATE(helper->program->product_memory, type)
#define HELPER_ALLOCATE_ARRAY(helper, type, c) MEMORY_ALLOCATE_ARRAY(helper->program->product_memory, type, c)

struct RuleWithParent* builder_make_rule_from_helper(struct SelectorHelper* helper);

//builders
struct DrawObj* builder_build_draw_obj(struct SelectorHelper* helper);
struct BasicObj builder_build_basic(struct RuleWithParent* rule, struct DrawObj* parent);
void builder_init_basic(struct BasicObj* basic);
struct BasicObj builder_build_empty_basic();
struct DrawObj* builder_build_void(struct Helper* helper);
struct DrawObj* builder_build_custom_void(
        struct Helper* helper,
        struct BasicObj basic,
        struct RuleSelector* child_selector);
struct BoardChild* builder_build_board_child(struct Helper* helper, struct DrawObj* obj, int x, int y);
struct DrawObj* builder_build_pyramid(struct Helper* helper);
struct DrawObj* builder_build_triangle(struct Helper* helper);
struct DrawObj* builder_build_cube(struct Helper* helper);
struct DrawObj* builder_build_cone(struct Helper* helper);
struct DrawObj* builder_build_cylinder(struct Helper* helper);
struct DrawObj* builder_build_dome(struct Helper* helper);

//builder containers
struct DrawObj* builder_build_bsp_tree(struct Helper* helper);
struct DrawObj* builder_build_square_fence(struct Helper* helper);
struct DrawObj* builder_build_series(struct Helper* helper, enum FillDirection fill_direction);
struct DrawObj* builder_build_filler(struct Helper* helper, enum FillDirection fill_direction);
struct DrawObj* builder_build_union(struct Helper* helper);

//builder textures
struct WallObj* builder_build_wall(struct SelectorHelper* helper, int wall_width, int wall_height);
// struct FloorObj* builder_build_floor(struct SelectorHelper* helper, int wall_width);
// struct TexObj* builder_build_texture(struct SelectorHelper* helper);

//utils
struct DrawObj* builder_make_draw_obj(struct Helper* helper, struct BasicObj basic, enum DrawObjType type, void* obj);
struct TexObj* builder_make_texture(struct SelectorHelper* helper, int width, int height);

//series utils
struct ShiftDrawPair* series_make_pair(struct Helper* helper, int shift, struct DrawObj* obj);
struct DrawObj* series_make_draw_obj(struct Helper* helper, struct RuleSelector* selector);
int series_get_basic_metric_by_fill_direction(struct BasicObj *basic, enum FillDirection fill_direction);
void series_add_basic_metric_by_fill_direction(struct BasicObj *basic, enum FillDirection fill_direction, int x);
void series_add_max_basic_by_fill_direction(struct BasicObj *a, struct BasicObj *b, enum FillDirection fill_direction);
void series_max_basic(struct BasicObj *a, struct BasicObj *b);
void series_align_objs(
        struct Helper* helper,
        struct ShiftDrawPair** pairs,
        enum FillDirection fill_direction,
        size_t size, int end_width);

//inlines
static inline double builder_make_theta(const int rotate) {
    return rotate * M_PI / 180.0;
}

static inline int builder_compute_rotate(const int child_rotate, struct BasicObj* parent_basic) {
    int parent_rotate = parent_basic ? parent_basic->rotate : 0;
    return (child_rotate + parent_rotate) % 360;
}