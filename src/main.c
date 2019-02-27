#include <basic.h>
#include <stdlib.h>
#include <stdio.h>

#include "img.h"
#include "draw.h"
#include "draw_shape.h"
#include "css_draw.h"

#include "css_debug.h"

void do_sth(struct image *img) {
    struct Program *program = css_parse_file("example.css");
    //css_debug_program(stderr, program);
    int vox[3] = {128, 64, 64};
    struct RuleSelector query = default_selector;
    query.element = "wall";
    query.klass = "test";
    css_draw_wall(img, program, &query, vox, DIRECTION_SOUTH);
}

int main(int argc, char **argv) {
    struct image *img = create_black_image(800, 800);
    do_sth(img);
    write_png_file("out.png", img);
    return 0;
}
