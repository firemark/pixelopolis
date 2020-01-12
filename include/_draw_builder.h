#include "draw_builder.h" 
#include "hash.h"

#define BUILDER_MAX_ELEMENTS 128

struct Helper {
    struct Program* program;
    struct Rule* rule;
    struct DrawObj* parent;
};

struct SelectorHelper {
    struct Program* program;
    struct Rule* parent_rule;
    struct RuleSelector* selector;
    struct DrawObj* parent;
};

//builders
struct DrawObj* builder_build_draw_obj(struct SelectorHelper* helper);
struct BasicObj builder_build_basic(struct Rule* rule, struct DrawObj* parent);
struct BasicObj builder_build_empty_basic();
struct DrawObj* builder_build_void(struct Helper* helper);
struct DrawObj* builder_build_pyramid(struct Helper* helper);
struct DrawObj* builder_build_triangle(struct Helper* helper);
struct DrawObj* builder_build_cube(struct Helper* helper);
struct DrawObj* builder_build_series(struct Helper* helper, enum FillDirection fill_direction);
struct DrawObj* builder_build_filler(struct Helper* helper, enum FillDirection fill_direction);
struct WallObj* builder_build_wall(struct SelectorHelper* helper, int wall_width, int wall_height);
struct FloorObj* builder_build_floor(struct SelectorHelper* helper, int wall_width);
struct TexObj* builder_build_texture(struct SelectorHelper* helper);

//utils
struct Rule* builder_make_rule_from_helper(struct SelectorHelper* helper);
struct DrawObj* builder_make_draw_obj(struct Helper* helper, struct BasicObj basic, enum DrawObjType type, void* obj);
int builder_get_padding(struct Rule* rule);

//series utils
struct ShiftDrawPair* series_make_pair(int shift, struct DrawObj* obj);
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

//caches
extern struct HashMap* css_builder_cache_textures;
