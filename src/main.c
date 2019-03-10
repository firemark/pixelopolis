#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "basic.h"
#include "img.h"
#include "draw.h"
#include "draw_shape.h"
#include "css_func.h"
#include "css_draw.h"

#include "css_debug.h"

struct Rule* find_world(struct Program *program) {
    struct RuleSelector query = default_selector;
    query.element = "world";
    return css_find_rule_by_query(program, &query);
}

struct image* make_img(struct Rule *world) {
    int *width_ptr = css_find_number_prop(world, "width");
    int *height_ptr = css_find_number_prop(world, "height");
    int width = width_ptr ? *width_ptr : 400;
    int height = height_ptr ? *height_ptr : 400;

    return create_black_image(width, height);
}

void draw(struct Program *program, struct Rule *world, struct image *img) {
    int vox[3] = {32, 32, 64};
    struct RuleSelector* body_selector = css_find_selector_prop(world, "body");

    struct DrawInfo draw_info = {
        .img=img,
        .program=program,
        .query=body_selector,
        .vox=vox,
    };
    draw_component(&draw_info, NULL);
}

int main(int argc, char **argv) {
    char *in_filename = argc > 1 ? argv[1] : "-";
    char *out_filename = argc > 2 ? argv[2] : "out.png";

    if (!strcmp(in_filename, "-")) {
        in_filename = NULL;
    }

    struct Program *program = css_parse_file(in_filename);
    struct Rule *world_rule = find_world(program);
    if (!world_rule) {
        fprintf(stderr, "world rule not found! :(\n");
    }
    struct image *img = make_img(world_rule);
    draw(program, world_rule, img);

    if (!strcmp(out_filename, "-")) {
        write_png_file(stdout, img);
        return 0;
    } 

    FILE *fp = fopen(out_filename, "wb");
    if (!fp) {
        fprintf(stderr, "Problem with creating file %s\n", out_filename);
        return -1;
    }
    write_png_file(fp, img);
    fclose(fp);

    return 0;
}
