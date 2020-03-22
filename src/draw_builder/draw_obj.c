#include <string.h>
#include "_draw_builder.h"
#include "css_func.h"

struct DrawObj* builder_build_draw_obj(struct SelectorHelper* helper) {
    struct RuleSelector *query = helper->selector;
    if (!query) return NULL;
    struct Rule *rule = builder_make_rule_from_helper(helper);
    if (!rule) return NULL;

    struct Helper inner_helper = {
        .program=helper->program,
        .rule=rule,
        .parent=helper->parent,
    };

    struct RuleSelector *display_selector = css_find_selector_prop(rule, "display");
    char* element = display_selector ? display_selector->element : query->element;
    struct DrawObj *obj;
#define IF_NAME(str) else if (!strcmp(element, str))
    if (!element) obj = NULL; 
    // basic
    IF_NAME("cube")             obj = builder_build_cube(&inner_helper);
    IF_NAME("cone")             obj = builder_build_cone(&inner_helper);
    IF_NAME("cylinder")         obj = builder_build_cylinder(&inner_helper);
    //IF_NAME("dome")             obj = builder_build_dome(&inner_helper);
    //IF_NAME("trapezoid")        obj = builder_build_trapezoid(&inner_helper);
    //IF_NAME("stairs")           obj = builder_build_stairs(&inner_helper);
    IF_NAME("triangle")         obj = builder_build_triangle(&inner_helper);
    IF_NAME("pyramid")          obj = builder_build_pyramid(&inner_helper);
    IF_NAME("void")             obj = builder_build_void(&inner_helper);

    // advanced
    //IF_NAME("union")            obj = builder_build_union(&inner_helper);
    //IF_NAME("border-walls")     obj = builder_build_border_walls(&inner_helper);
    IF_NAME("v-series")         obj = builder_build_series(&inner_helper, VERTICAL_FILL);
    IF_NAME("d-series")         obj = builder_build_series(&inner_helper, DEPTH_FILL);
    IF_NAME("h-series")         obj = builder_build_series(&inner_helper, HORIZONTAL_FILL);
    IF_NAME("v-filler")         obj = builder_build_filler(&inner_helper, VERTICAL_FILL);
    IF_NAME("d-filler")         obj = builder_build_filler(&inner_helper, DEPTH_FILL);
    IF_NAME("h-filler")         obj = builder_build_filler(&inner_helper, HORIZONTAL_FILL);
    else obj = NULL;
#undef IF_NAME

    css_free_rule_half(rule);
    return obj;
}
