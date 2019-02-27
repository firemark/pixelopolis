#include <basic.h>
#include <stdlib.h>
#include <stdio.h>

#include "img.h"
#include "draw.h"
#include "draw_shape.h"
#include "css_draw.h"

#include "css_debug.h"

void do_sth(struct image *img) {
    struct Program *program = css_parse_file("test.css");
    //css_debug_program(stderr, program);
    int vox[3] = {128, 64, 64};
    struct RuleSelector query = default_selector;
    query.element = "cube";
    query.klass = "test";
    css_draw_cube(img, program, &query, vox);
}

int main(int argc, char **argv) {
    struct image *img = create_black_image(400, 400);
    do_sth(img);
    write_png_file("out.png", img);
    return 0;
}
