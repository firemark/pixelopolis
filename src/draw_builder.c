#include <stdio.h>

#include "css_func.h"
#include "draw_builder.h"

int _check_element_name(struct RuleSelector *selector, char *name) {
    return strcmp(selector->element, name) == 0;
}

struct SeriesObj* build_series(struct Program* program, struct Rule* rule, enum Series series) {
    int *width_ptr = css_find_number_prop(rule, "width");
    int *height_ptr = css_find_number_prop(rule, "height");
    int *depth_ptr = css_find_number_prop(rule, "depth");
    int *padding = css_find_number_prop(rule, "padding");

    struct BasicObj basic = {
        .width=width_ptr ? *width_ptr : 50,
        .height=height_ptr ? *height_ptr : 50,
        .depth=depth_ptr ? *depth_ptr : 50,
        .rotation=0,
    }

    struct SeriesObj* obj = malloc(sizeof(struct SeriesObj));
    obj->basic = basic;
    obj->series = series;

    return obj;
}

