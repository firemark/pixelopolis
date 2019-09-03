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
    struct DrawObj *parent;
};

struct DrawObj* _build_pyramid(struct Helper* helper);
struct DrawObj* _build_triangle(struct Helper* helper);
struct DrawObj* _build_cube(struct Helper* helper);
struct DrawObj* _build_series(struct Helper* helper, enum Series series);

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
struct BasicObj* _get_basic_pointer(struct DrawObj* draw_obj) {
    void* obj = draw_obj->obj;
    switch (draw_obj->type) {
        case DRAW_OBJ_CUBE: return &((struct CubeObj*)obj)->basic;
        case DRAW_OBJ_TRIANGLE: return &((struct TriangleObj*)obj)->basic;
        case DRAW_OBJ_PYRAMID: return &((struct PyramidObj*)obj)->basic;
        case DRAW_OBJ_SERIES: return &((struct SeriesObj*)obj)->basic;
        default: return NULL;
    }
}


struct BasicObj _build_basic(struct Rule* rule, struct DrawObj* parent) {
    int* rotation_ptr = css_find_number_prop(rule, "rotation");
    struct BasicObj* parent_basic = parent ? _get_basic_pointer(parent) : NULL;

    struct BasicObj basic = {
        .width=_get_basic_metric_width(rule, 50, parent_basic),
        .height=_get_basic_metric_height(rule, 50, parent_basic),
        .depth=_get_basic_metric_depth(rule, 50, parent_basic),
        .rotation=rotation_ptr ? *rotation_ptr : 0,
    };

    return basic;
}

int _check_el_name(struct RuleSelector *selector, char *name) {
    return strcmp(selector->element, name) == 0;
}

struct DrawObj* _build_draw_obj(struct Helper* helper) {
    struct Helper inner_helper = {
        .program=helper->program,
        .rule=helper->rule,
        .parent=helper->parent,
    };

    struct RuleSelector* query = helper->rule->selector;
    if (_check_el_name(query, "cube")) {
        return _build_cube(&inner_helper);
    } else if (_check_el_name(query, "triangle")) {
        return _build_triangle(&inner_helper);
    } else if (_check_el_name(query, "pyramid")) {
        return _build_pyramid(&inner_helper);
    } else if (_check_el_name(query, "v-series")) {
        return _build_series(&inner_helper, VERTICAL_SERIES);
    } else if (_check_el_name(query, "h-series")) {
        return _build_series(&inner_helper, HORIZONTAL_SERIES);
    } else if (_check_el_name(query, "d-series")) {
        return _build_series(&inner_helper, DEPTH_SERIES);
    } else {
        return NULL;
    }
}

struct DrawObj** _build_many_objs(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    struct Prop* body_prop = css_find_prop(rule, "body");
    struct Obj* obj = NULL;
    int size = 0;
    css_iter(obj, body_prop->objs) size++; // counter

    struct DrawObj **objs = malloc(sizeof(struct DrawObj*) * (size + 1));
    int i = 0;
    css_iter(obj, body_prop->objs) {
        struct RuleSelector* selector = css_eval_rule(obj);
        if (!selector) continue;
        struct Rule* inner_rule = css_find_rule_by_query(helper->program, selector);
        struct Helper inner_helper = {
            .program=helper->program,
            .rule=inner_rule,
            .parent=helper->parent,
        };
        objs[i++] = _build_draw_obj(&inner_helper);
    }

    objs = realloc(objs, sizeof(struct DrawObj*) * (i + 1));
    objs[i] = NULL;

    return objs;
}

struct DrawObj* _build_series(struct Helper* helper, enum Series series) {
    struct Rule *rule = helper->rule;
    int *padding_ptr = css_find_number_prop(rule, "padding");

    struct SeriesObj* obj = malloc(sizeof(struct SeriesObj));
    obj->basic = _build_basic(rule, helper->parent);
    obj->padding = padding_ptr? *padding_ptr : 0;
    obj->series = series;

    struct DrawObj *draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->type = DRAW_OBJ_SERIES;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=helper->rule,
        .parent=draw_obj,
    };
    obj->objs = _build_many_objs(&inner_helper);

    return draw_obj;
}

struct TexObj* _build_texture(struct Helper* helper) {
    struct Rule* rule = helper->rule;
    if (!rule) return NULL;
    char* filename = css_find_string_prop(rule, "texture"); 
    if (!filename) return NULL;

    struct TexObj* obj = malloc(sizeof(struct TexObj));
    obj->texture = filename ? _builder_get_texture(filename) : NULL;
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

    struct Helper left_helper = {
        .program=program,
        .rule=css_find_rule_prop(program, rule, "left"),
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

    struct Helper right_helper = {
        .program=program,
        .rule=css_find_rule_prop(program, rule, "right"),
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
        struct Helper middle_helper = {
            .program=program,
            .rule=css_find_rule_prop(program, rule, "middle"),
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

struct FloorObj* _build_floor(struct Helper* helper, int wall_width) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    int* height_ptr = css_find_number_prop(rule, "height");
    int* padding_ptr = css_find_number_prop(rule, "padding");

    struct FloorObj* floor = malloc(sizeof(struct FloorObj));
    floor->padding = padding_ptr ? *padding_ptr : 0;
    floor->height = _get_floor_height(height_ptr, floor);

    struct Helper tex_helper = {
        .program=helper->program,
        .rule=css_find_rule_prop(helper->program, rule, "texture"),
        .parent=helper->parent,
    };
    floor->tex = _build_texture(&tex_helper);
    _append_objs_to_floor(helper, floor, wall_width);

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

    struct Helper bottom_helper = {
        .program=program,
        .rule=css_find_rule_prop(program, rule, "bottom"),
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

    struct Helper top_helper = {
        .program=program,
        .rule=css_find_rule_prop(program, rule, "top"),
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
        struct Helper middle_helper = {
            .program=program,
            .rule=css_find_rule_prop(program, rule, "middle"),
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

struct WallObj* _build_wall(struct Helper* helper, int wall_width, int wall_height) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct WallObj *wall = malloc(sizeof(struct WallObj));
    int* padding_ptr = css_find_number_prop(rule, "padding");
    int padding = padding_ptr? *padding_ptr: 0;

    wall->padding = padding;

    struct Helper tex_helper = {
        .program=helper->program,
        .rule=css_find_rule_prop(helper->program, rule, "texture"),
        .parent=helper->parent,
    };
    wall->tex = _build_texture(&tex_helper);
    _append_floors_to_wall(helper, wall, wall_width, wall_height);
    return wall;
}

struct DrawObj* _build_cube(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct CubeObj* obj = malloc(sizeof(struct CubeObj));
    obj->basic = _build_basic(rule, helper->parent);
    struct BasicObj* basic = &obj->basic;

    struct Helper wall_helper = {
        .program=helper->program,
        .rule=css_find_rule_prop(helper->program, rule, "wall"),
        .parent=helper->parent,
    };
    obj->south_wall = _build_wall(&wall_helper, basic->width, basic->height);
    obj->east_wall = _build_wall(&wall_helper, basic->depth, basic->height);
    struct Helper roof_helper = {
        .program=helper->program,
        .rule=css_find_rule_prop(helper->program, rule, "roof"),
        .parent=helper->parent,
    };
    obj->roof = _build_wall(&roof_helper, basic->width, basic->depth);

    struct DrawObj* draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->type = DRAW_OBJ_CUBE;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    return draw_obj;
}

struct DrawObj* _build_triangle(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct TriangleObj* obj = malloc(sizeof(struct TriangleObj));
    obj->basic = _build_basic(rule, helper->parent);
    struct BasicObj* basic = &obj->basic;

    struct Helper wall_helper = {
        .program=helper->program,
        .rule=css_find_rule_prop(helper->program, rule, "wall"),
        .parent=helper->parent,
    };
    obj->wall = _build_wall(&wall_helper, basic->width, basic->height);
    struct Helper roof_helper = {
        .program=helper->program,
        .rule=css_find_rule_prop(helper->program, rule, "roof"),
        .parent=helper->parent,
    };
    obj->roof = _build_wall(&roof_helper, basic->width, basic->depth);

    struct DrawObj* draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->type = DRAW_OBJ_TRIANGLE;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    return draw_obj;
}

struct DrawObj* _build_pyramid(struct Helper* helper) {
    struct Rule *rule = helper->rule;
    if (!rule) return NULL;
    struct PyramidObj* obj = malloc(sizeof(struct PyramidObj));
    obj->basic = _build_basic(rule, helper->parent);
    struct BasicObj* basic = &obj->basic;

    struct Helper wall_helper = {
        .program=helper->program,
        .rule=css_find_rule_prop(helper->program, rule, "wall"),
        .parent=helper->parent,
    };
    obj->south_wall = _build_wall(&wall_helper, basic->width, basic->height);
    obj->east_wall = _build_wall(&wall_helper, basic->depth, basic->height);

    struct DrawObj* draw_obj = malloc(sizeof(struct DrawObj));
    draw_obj->type = DRAW_OBJ_PYRAMID;
    draw_obj->obj = obj;
    draw_obj->parent = helper->parent;

    return draw_obj;
}

struct DrawObj* builder_make(struct Program* program, struct Rule* world) {
    struct Rule* body = css_find_rule_prop(program, world, "body");
    struct Helper helper = {
        .program=program,
        .rule=body,
        .parent=NULL,
    };
    return _build_draw_obj(&helper);
}
