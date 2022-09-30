#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pixelopolis/basic.h"
#include "pixelopolis/css_debug.h"
#include "pixelopolis/css_draw.h"
#include "pixelopolis/css_eval.h"
#include "pixelopolis/css_func.h"
#include "pixelopolis/draw.h"
#include "pixelopolis/draw_builder.h"
#include "pixelopolis/img.h"

struct Rule *find_world(struct Program *program) {
    struct RuleSelector query = default_selector;
    query.element = "world";
    return css_find_rule_by_query(program, &query);
}

struct image *make_img(struct Rule *world) {
    int *width_ptr = css_find_number_prop(world, "width");
    int *height_ptr = css_find_number_prop(world, "height");
    int width = width_ptr ? *width_ptr : 400;
    int height = height_ptr ? *height_ptr : 400;

    return create_black_image(width, height);
}

enum TexFilterAlgorithm get_filter_algorithm(struct Rule *world) {
    char *name = css_find_selector_element_prop(world, "texture-filter");
    enum TexFilterAlgorithm filter = FILTER_ALGORITHM_NONE;
    if (!name) return filter;
#define IFMATCH(s) else if (!strcmp(name, s))
    IFMATCH("none") filter = FILTER_ALGORITHM_NONE;
    IFMATCH("scale2") filter = FILTER_ALGORITHM_SCALE2;
    IFMATCH("scale3") filter = FILTER_ALGORITHM_SCALE3;
    IFMATCH("mame2") filter = FILTER_ALGORITHM_MAME2;
    IFMATCH("mame4") filter = FILTER_ALGORITHM_MAME4;
#undef IFMATCH
    free(name);
    return filter;
}

void draw(struct DrawObj *draw_obj, struct Rule *world, struct image *img) {
    int vox[3] = {0, 0, 0};
    struct DrawInfo draw_info = {
        .img = img,
        .vox = vox,
        .filter = get_filter_algorithm(world),
    };
    draw_component(draw_obj, &draw_info, NULL);
}

int main(int argc, char **argv) {
    char *in_filename = argc > 1 ? argv[1] : "-";
    char *out_filename = argc > 2 ? argv[2] : "out.png";

    builder_init();
    css_eval_start();
    struct Program *program;
    if (!strcmp(in_filename, "-")) {
        program = css_parse_file_as_stream(stdin);
    } else {
        program = css_parse_file(in_filename);
    }
    // css_debug_program(stderr, program);
    struct Rule *world_rule = find_world(program);
    if (!world_rule) {
        fprintf(stderr, "world rule not found! :(\n");
    }
    struct DrawObj *draw_obj = builder_make(program, world_rule);
    struct image *img = make_img(world_rule);
    draw(draw_obj, world_rule, img);

    FILE *fp = strcmp(out_filename, "-") ? fopen(out_filename, "wb") : stdout;
    if (!fp) {
        fprintf(stderr, "Problem with creating file %s\n", out_filename);
        return -1;
    }
    write_png_file_from_image(fp, img);
    fclose(fp);
    builder_stop();
    css_eval_stop();
    css_free_program(program);
    destroy_image(img);

    return 0;
}
