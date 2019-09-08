#include <stdlib.h>
#include <string.h>

#include "css_func.h"
#include "css_eval.h"
#include "draw_builder.h"
#include "hash.h"
#include "img.h"

#define MAX_ELEMENTS 128

struct Helper {
    struct Program* program;
    struct Rule* rule;
    struct DrawObj* parent;
};

struct SelectorHelper {
    struct Program* program;
    struct RuleSelector* selector;
    struct DrawObj* parent;
};

struct DrawObj* _build_void(struct Helper* helper);
struct DrawObj* _build_pyramid(struct Helper* helper);
struct DrawObj* _build_triangle(struct Helper* helper);
struct DrawObj* _build_cube(struct Helper* helper);
struct DrawObj* _build_series(struct Helper* helper, enum FillDirection fill_direction);
struct DrawObj* _build_filler(struct Helper* helper, enum FillDirection fill_direction);

struct HashMap* cache_textures;

void builder_init(void) {
    cache_textures = hash_make();
}

void builder_stop(void) {
    struct image* texture;
    hash_iter_values(texture, cache_textures) {
        destroy_image(texture);
    }
    hash_destroy(cache_textures);
}

struct FlatImage* _builder_get_texture(char *filename) {
    struct FlatImage* texture;

    texture = hash_get(cache_textures, filename);
    if (texture) {
        return texture;
    }

    texture = flat_image_read_png_file(filename);
    if (!texture) {
        return NULL;
    }

    hash_set(cache_textures, filename, texture, NULL);
    return texture;
}

struct Rule* _css_make_rule_from_helper(struct SelectorHelper *helper) {
    if (!helper->selector) return NULL;
    return css_make_rule_from_selector(helper->program, helper->selector);
}

#define MAKE_GET_METRIC(NAME, ATTR, PARENT_TYPE) \
int NAME ## _ ## ATTR(struct Rule* rule, int default_value, PARENT_TYPE* parent) { \
    struct Obj* obj = css_find_1st_obj(rule, #ATTR); \
    if (!obj) return default_value; \
    if (obj->type == OBJ_NUMBER) return *(int*)obj->value; \
    if (obj->type != OBJ_PERCENT) return default_value; \
    if (!parent) return default_value; \
    int percent = *(int*)obj->value; \
    int parent_value = parent->ATTR; \
    return (percent * parent_value) / 100; \
}
MAKE_GET_METRIC(_get_basic_metric, width, struct BasicObj)
MAKE_GET_METRIC(_get_basic_metric, height, struct BasicObj)
MAKE_GET_METRIC(_get_basic_metric, depth, struct BasicObj)

struct BasicObj _build_basic(struct Rule* rule, struct DrawObj* parent) {
    struct BasicObj* parent_basic = parent ? &parent->basic : NULL;

    struct BasicObj basic = {
        .width=_get_basic_metric_width(rule, 50, parent_basic),
        .height=_get_basic_metric_height(rule, 50, parent_basic),
        .depth=_get_basic_metric_depth(rule, 50, parent_basic),
    };

    return basic;
}

struct BasicObj _build_empty_basic() {
    struct BasicObj basic = {
        .width=0,
        .height=0,
        .depth=0,
    };

    return basic;
}

int _get_basic_metric_by_fill_direction(struct BasicObj *basic, enum FillDirection fill_direction) {
    switch(fill_direction) {
        case VERTICAL_FILL: return basic->width;
        case HORIZONTAL_FILL: return basic->height;
        case DEPTH_FILL: return basic->depth;
        default: return 0;
    }
}

int _add_basic_metric_by_fill_direction(struct BasicObj *basic, enum FillDirection fill_direction, int x) {
    switch(fill_direction) {
        case VERTICAL_FILL: basic->width += x; break;
        case HORIZONTAL_FILL: basic->height += x; break;
        case DEPTH_FILL: basic->depth += x; break;
    }
}

int _add_max_basic_by_fill_direction(struct BasicObj *a, struct BasicObj *b, enum FillDirection fill_direction) {
    switch(fill_direction) {
        case VERTICAL_FILL: 
            a->width += b->width;
            a->height = a->height >= b->height ? a->height : b->height;
            a->depth = a->depth >= b->depth ? a->depth : b->depth;
            break;
        case HORIZONTAL_FILL: 
            a->height += b->height;
            a->width = a->width >= b->width ? a->width : b->width;
            a->depth = a->depth >= b->depth ? a->depth : b->depth;
            break;
        case DEPTH_FILL: 
            a->depth += a->depth;
            a->width = a->width >= b->width ? a->width : b->width;
            a->height = a->height >= b->height ? a->height : b->height;
            break;
    }
}

void _max_basic(struct BasicObj *a, struct BasicObj *b) {
    a->width = a->width >= b->width ? a->width : b->width;
    a->height = a->height >= b->height ? a->height : b->height;
    a->depth = a->depth >= b->depth ? a->depth : b->depth;
}

int _check_el_name(struct RuleSelector *selector, char *name) {
    return strcmp(selector->element, name) == 0;
}

struct DrawObj* _build_draw_obj(struct SelectorHelper* helper) {
    struct RuleSelector *query = helper->selector;
    if (!query) return NULL;
    struct Rule *rule = _css_make_rule_from_helper(helper);
    if (!rule) return NULL;

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=rule,
        .parent=helper->parent,
    };

    struct DrawObj *obj;
    if (_check_el_name(query, "void")) {
        obj = _build_void(&inner_helper);
    } else if (_check_el_name(query, "cube")) {
        obj = _build_cube(&inner_helper);
    } else if (_check_el_name(query, "triangle")) {
        obj = _build_triangle(&inner_helper);
    } else if (_check_el_name(query, "pyramid")) {
        obj = _build_pyramid(&inner_helper);
    } else if (_check_el_name(query, "v-series")) {
        obj = _build_series(&inner_helper, VERTICAL_FILL);
    } else if (_check_el_name(query, "h-series")) {
        obj = _build_series(&inner_helper, HORIZONTAL_FILL);
    } else if (_check_el_name(query, "d-series")) {
        obj = _build_series(&inner_helper, DEPTH_FILL);
    } else if (_check_el_name(query, "v-filler")) {
        obj = _build_filler(&inner_helper, VERTICAL_FILL);
    } else if (_check_el_name(query, "h-filler")) {
        obj = _build_filler(&inner_helper, HORIZONTAL_FILL);
    } else if (_check_el_name(query, "d-filler")) {
        obj = _build_filler(&inner_helper, DEPTH_FILL);
    } else {
        obj = NULL;
    }

    css_free_rule_half(rule);
    return obj;
}

struct DrawObj** _build_many_objs(struct SeriesObj* series, struct Helper* helper) {
    struct Rule *rule = helper->rule;
    struct Obj **prop_objs = css_find_objs(rule, "body");
    struct Obj* obj = NULL;
    int size = 0;
    css_iter(obj, prop_objs) size++; // counter

    struct BasicObj basic_temp = _build_empty_basic();
    struct DrawObj **objs = malloc(sizeof(struct DrawObj*) * (size + 1));
    int i = 0;
    css_iter(obj, prop_objs) {
        struct RuleSelector* selector = css_eval_rule(obj);
        if (!selector) continue;

        struct SelectorHelper inner_helper = {
            .program=helper->program,
            .selector=selector,
            .parent=helper->parent,
        };
        struct DrawObj *child = _build_draw_obj(&inner_helper);
        objs[i++] = child;
        _add_max_basic_by_fill_direction(&basic_temp, &child->basic, series->fill_direction);
    }

    objs = realloc(objs, sizeof(struct DrawObj*) * (i + 1));
    objs[i] = NULL;

    _max_basic(&helper->parent->basic, &basic_temp); // resize parent
    return objs;
}

struct DrawObj* _build_series(struct Helper* helper, enum FillDirection fill_direction) {
    struct Rule *rule = helper->rule;
    int *padding_ptr = css_find_number_prop(rule, "padding");

    struct SeriesObj* obj = malloc(sizeof(struct SeriesObj));
    obj->padding = padding_ptr? *padding_ptr : 0;
    obj->fill_direction = fill_direction;
    obj->left = NULL;
    obj->right = NULL;

    struct DrawObj *draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->basic = _build_basic(rule, helper->parent);
    draw_obj->type = DRAW_OBJ_SERIES;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=helper->rule,
        .parent=draw_obj,
    };
    obj->objs = _build_many_objs(obj, &inner_helper);

    return draw_obj;
}

void _append_objs_to_filler(struct Helper* helper, struct SeriesObj* filler, int width) {
    int x = 0;
    int size = 0;
    int padding = filler->padding;
    struct DrawObj** objs = malloc(sizeof(struct DrawObj*) * MAX_ELEMENTS);
    struct Rule* rule = helper->rule;
    struct Program* program = helper->program;
    enum FillDirection fill_direction = filler->fill_direction;

    struct DrawObj* left = NULL; 
    struct DrawObj* right = NULL;

    struct BasicObj basic_temp = _build_empty_basic();

    struct RuleSelector* left_selector = css_find_selector_prop(rule, "left");
    struct SelectorHelper left_helper = {
        .program=helper->program,
        .selector=left_selector,
        .parent=helper->parent,
    };
    left = _build_draw_obj(&left_helper);
    if (left) {
        int left_width = _get_basic_metric_by_fill_direction(&left->basic, fill_direction);
        if (left_width < width) {
            x += left_width + padding;
            _add_max_basic_by_fill_direction(&basic_temp, &left->basic, fill_direction);
            _add_basic_metric_by_fill_direction(&basic_temp, fill_direction, padding);
        } else {
            left = NULL;
            right = NULL;
            goto final;
        }
    }

    struct RuleSelector* right_selector = css_find_selector_prop(rule, "right");
    struct SelectorHelper right_helper = {
        .program=helper->program,
        .selector=right_selector,
        .parent=helper->parent,
    };
    right = _build_draw_obj(&right_helper);
    if (right) {
        int right_width = _get_basic_metric_by_fill_direction(&right->basic, fill_direction);
        if (x + right_width < width) {
            width -= right_width;
            _add_max_basic_by_fill_direction(&basic_temp, &right->basic, fill_direction);
        } else {
            right = NULL;
            goto final;
        }
    }

    for(;;) {
        struct RuleSelector* middle_selector = css_find_selector_prop(rule, "body");
        struct SelectorHelper middle_helper = {
            .program=helper->program,
            .selector=middle_selector,
            .parent=helper->parent,
        };
        struct DrawObj* middle = _build_draw_obj(&middle_helper);
        int middle_width;
        if (middle) {
            middle_width = _get_basic_metric_by_fill_direction(&middle->basic, fill_direction);
        } else {
            middle_width = 12; 
        }
        if (x + middle_width >= width) break;

        if (middle) {
            _add_max_basic_by_fill_direction(&basic_temp, &middle->basic, fill_direction);
        } else {
            _add_basic_metric_by_fill_direction(&basic_temp, fill_direction, 12);
        }
        _add_basic_metric_by_fill_direction(&basic_temp, fill_direction, padding);
        x += middle_width + padding;
        objs[size++] = middle;
    }

final:
    _max_basic(&helper->parent->basic, &basic_temp); // resize parent
    objs = realloc(objs, sizeof(struct DrawObj*) * (size + 1));
    objs[size] = NULL;
    filler->objs = objs;
    filler->right = right;
    filler->left = left;
}

struct DrawObj* _build_filler(struct Helper* helper, enum FillDirection fill_direction) {
    struct Rule *rule = helper->rule;
    int *padding_ptr = css_find_number_prop(rule, "padding");

    struct SeriesObj* obj = malloc(sizeof(struct SeriesObj));
    obj->padding = padding_ptr? *padding_ptr : 0;
    obj->fill_direction = fill_direction;

    struct DrawObj *draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->basic = _build_basic(rule, helper->parent);
    draw_obj->type = DRAW_OBJ_SERIES;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=rule,
        .parent=draw_obj,
    };

    int width = _get_basic_metric_by_fill_direction(&draw_obj->basic, fill_direction);
    _append_objs_to_filler(&inner_helper, obj, width);

    return draw_obj;
}

struct TexObj* _build_texture(struct SelectorHelper* helper) {
    struct Rule* rule = _css_make_rule_from_helper(helper);
    if (!rule) return NULL;
    char* filename = css_find_string_prop(rule, "texture"); 
    if (!filename) return NULL;

    struct TexObj* obj = malloc(sizeof(struct TexObj));
    obj->texture = filename ? _builder_get_texture(filename) : NULL;

    css_free_rule_half(rule);
    return obj;
}

int _get_floor_height(int *height_ptr, struct FloorObj* obj) {
    if (height_ptr) {
        return *height_ptr;
    }  
    if (obj->tex && obj->tex->texture) {
        return obj->tex->texture->height;
    } 

    return 0;
}

void _append_objs_to_floor(struct Helper* helper, struct FloorObj* floor, int wall_width) {
    int x = 0;
    int size = 0;
    int padding = floor->padding;
    struct TexObj** objs = malloc(sizeof(struct TexObj*) * MAX_ELEMENTS);
    struct Rule* rule = helper->rule;
    struct Program* program = helper->program;

    struct TexObj* left = NULL; 
    struct TexObj* right = NULL;

    struct SelectorHelper left_helper = {
        .program=program,
        .selector=css_find_selector_prop(rule, "left"),
        .parent=helper->parent,
    };
    left = _build_texture(&left_helper);
    if (left && left->texture) {
        if (left->texture->width < wall_width) {
            x += left->texture->width + padding;
        } else {
            left = NULL;
            right = NULL;
            goto final;
        }
    }

    struct SelectorHelper right_helper = {
        .program=program,
        .selector=css_find_selector_prop(rule, "right"),
        .parent=helper->parent,
    };
    right = _build_texture(&right_helper);
    if (right && right->texture) {
        if (x + right->texture->width < wall_width) {
            wall_width -= right->texture->width;
        } else {
            right = NULL;
            goto final;
        }
    }

    for(;;) {
        struct SelectorHelper middle_helper = {
            .program=program,
            .selector=css_find_selector_prop(rule, "middle"),
            .parent=helper->parent,
        };
        struct TexObj* middle = _build_texture(&middle_helper);
        int middle_width = middle && middle->texture ? middle->texture->width : 12;
        if (x + middle_width >= wall_width) break;
        x += middle_width + padding;
        objs[size++] = middle;
    }

final:
    objs[size] = NULL;
    floor->objs = objs;
    floor->objs_size = size;
    floor->right = right;
    floor->left = left;
}

struct FloorObj* _build_floor(struct SelectorHelper* helper, int wall_width) {
    struct Rule *rule = _css_make_rule_from_helper(helper);
    if (!rule) return NULL;
    int* height_ptr = css_find_number_prop(rule, "height");
    int* padding_ptr = css_find_number_prop(rule, "padding");

    struct FloorObj* floor = malloc(sizeof(struct FloorObj));
    floor->padding = padding_ptr ? *padding_ptr : 0;
    floor->height = _get_floor_height(height_ptr, floor);

    struct SelectorHelper tex_helper = {
        .program=helper->program,
        .selector=css_find_selector_prop(rule, "texture"),
        .parent=helper->parent,
    };
    floor->tex = _build_texture(&tex_helper);

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=rule,
        .parent=helper->parent,
    };
    _append_objs_to_floor(&inner_helper, floor, wall_width);

    css_free_rule_half(rule);
    return floor;
}

void _append_floors_to_wall(struct Helper* helper, struct WallObj* wall, int wall_width, int wall_height) {
    int y = 0;
    int size = 0;
    int padding = wall->padding;
    struct FloorObj** floors = malloc(sizeof(struct FloorObj*) * MAX_ELEMENTS);
    struct Rule* rule = helper->rule;
    struct Program* program = helper->program;

    struct FloorObj* bottom = NULL;
    struct FloorObj* top = NULL;

    struct SelectorHelper bottom_helper = {
        .program=program,
        .selector=css_find_selector_prop(rule, "bottom"),
        .parent=helper->parent,
    };
    bottom = _build_floor(&bottom_helper, wall_width);
    if (bottom) {
        if (bottom->height < wall_width) {
            y += bottom->height + padding;
        } else {
            bottom = NULL;
            top = NULL;
            goto final;
        }
    }

    struct SelectorHelper top_helper = {
        .program=program,
        .selector=css_find_selector_prop(rule, "top"),
        .parent=helper->parent,
    };
    top = _build_floor(&top_helper, wall_width);
    if (top) {
        if (y + top->height < wall_height) {
            wall_height -= top->height;
        } else {
            top = NULL;
            goto final;
        }
    }

    for(;;) {
        struct SelectorHelper middle_helper = {
            .program=program,
            .selector=css_find_selector_prop(rule, "middle"),
            .parent=helper->parent,
        };
        struct FloorObj* middle = _build_floor(&middle_helper, wall_width);
        int middle_height = middle ? middle->height : 12;
        if (y + middle_height >= wall_height) break;
        y += middle_height + padding;
        floors[size++] = middle;
    }

final:
    floors[size] = NULL;
    wall->floors = floors;
    wall->floors_size = size;
    wall->bottom = bottom;
    wall->top = top;
}

struct WallObj* _build_wall(struct SelectorHelper* helper, int wall_width, int wall_height) {
    struct Rule *rule = _css_make_rule_from_helper(helper);
    if (!rule) return NULL;
    struct WallObj *wall = malloc(sizeof(struct WallObj));
    int* padding_ptr = css_find_number_prop(rule, "padding");
    int padding = padding_ptr? *padding_ptr: 0;

    wall->padding = padding;

    struct SelectorHelper tex_helper = {
        .program=helper->program,
        .selector=css_find_selector_prop(rule, "texture"),
        .parent=helper->parent,
    };
    wall->tex = _build_texture(&tex_helper);
    struct Helper inner_helper = {
        .program=helper->program,
        .rule=rule,
        .parent=helper->parent,
    };
    _append_floors_to_wall(&inner_helper, wall, wall_width, wall_height);

    css_free_rule_half(rule);
    return wall;
}

struct DrawObj* _build_void(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct VoidObj* obj = malloc(sizeof(struct VoidObj));

    struct DrawObj *draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->basic = _build_basic(rule, helper->parent);
    draw_obj->type = DRAW_OBJ_VOID;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    struct RuleSelector* child_selector = css_find_selector_prop(rule, "child");
    struct SelectorHelper child_helper = {
        .program=helper->program,
        .selector=child_selector,
        .parent=helper->parent,
    };
    obj->child = _build_draw_obj(&child_helper);

    return draw_obj;
}

struct DrawObj* _build_cube(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct CubeObj* obj = malloc(sizeof(struct CubeObj));
    struct BasicObj basic = _build_basic(rule, helper->parent);

    struct SelectorHelper wall_helper = {
        .program=helper->program,
        .selector=css_find_selector_prop(rule, "wall"),
        .parent=helper->parent,
    };
    obj->south_wall = _build_wall(&wall_helper, basic.width, basic.height);
    obj->east_wall = _build_wall(&wall_helper, basic.depth, basic.height);
    struct SelectorHelper roof_helper = {
        .program=helper->program,
        .selector=css_find_selector_prop(rule, "roof"),
        .parent=helper->parent,
    };
    obj->roof = _build_wall(&roof_helper, basic.width, basic.depth);

    struct DrawObj* draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->basic = basic;
    draw_obj->type = DRAW_OBJ_CUBE;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    return draw_obj;
}

struct DrawObj* _build_triangle(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct TriangleObj* obj = malloc(sizeof(struct TriangleObj));
    struct BasicObj basic = _build_basic(rule, helper->parent);

    struct SelectorHelper wall_helper = {
        .program=helper->program,
        .selector=css_find_selector_prop(rule, "wall"),
        .parent=helper->parent,
    };
    obj->wall = _build_wall(&wall_helper, basic.width, basic.height);
    struct SelectorHelper roof_helper = {
        .program=helper->program,
        .selector=css_find_selector_prop(rule, "roof"),
        .parent=helper->parent,
    };
    obj->roof = _build_wall(&roof_helper, basic.width, basic.depth);

    struct DrawObj* draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->basic = basic;
    draw_obj->type = DRAW_OBJ_TRIANGLE;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    return draw_obj;
}

struct DrawObj* _build_pyramid(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct PyramidObj* obj = malloc(sizeof(struct PyramidObj));
    struct BasicObj basic = _build_basic(rule, helper->parent);

    struct SelectorHelper wall_helper = {
        .program=helper->program,
        .selector=css_find_selector_prop(rule, "wall"),
        .parent=helper->parent,
    };
    obj->south_wall = _build_wall(&wall_helper, basic.width, basic.height);
    obj->east_wall = _build_wall(&wall_helper, basic.depth, basic.height);

    struct DrawObj* draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->basic = basic;
    draw_obj->type = DRAW_OBJ_PYRAMID;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    return draw_obj;
}

struct DrawObj* builder_make(struct Program* program, struct Rule* world) {
    struct RuleSelector* query = css_find_selector_prop(world, "body");
    struct SelectorHelper helper = {
        .program=program,
        .selector=query,
        .parent=NULL,
    };
    return _build_draw_obj(&helper);
}
