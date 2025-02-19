#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pixelopolis/basic.h"
#include "pixelopolis/css/debug.h"
#include "pixelopolis/css/eval.h"
#include "pixelopolis/css/func.h"
#include "pixelopolis/draw_builder_texture.h"
#include "pixelopolis/draw_builder_world.h"
#include "pixelopolis/img.h"
#include "pixelopolis/img/draw.h"
#include "pixelopolis/img/royal_to_flat.h"
#include "pixelopolis/world_draw.h"

static struct Rule *find_world(struct Program *program) {
    struct RuleSelector query = default_selector;
    query.element = "world";
    return css_find_rule_by_query(program, &query);
}

static struct image *make_img(struct Rule *world) {
    int *width_ptr = css_find_number_prop(world, "width");
    int *height_ptr = css_find_number_prop(world, "height");
    int width = width_ptr ? *width_ptr : 400;
    int height = height_ptr ? *height_ptr : 400;

    return create_black_image(width, height);
}

// enum TexFilterAlgorithm get_filter_algorithm(struct Rule *world) {
//     char *name = css_find_selector_element_prop(world, "texture-filter");
//     enum TexFilterAlgorithm filter = FILTER_ALGORITHM_NONE;
//     if (!name) return filter;
// #define IFMATCH(s) else if (!strcmp(name, s))
//     IFMATCH("none") filter = FILTER_ALGORITHM_NONE;
//     IFMATCH("scale2") filter = FILTER_ALGORITHM_SCALE2;
//     IFMATCH("scale3") filter = FILTER_ALGORITHM_SCALE3;
//     IFMATCH("mame2") filter = FILTER_ALGORITHM_MAME2;
//     IFMATCH("mame4") filter = FILTER_ALGORITHM_MAME4;
// #undef IFMATCH
//     free(name);
//     return filter;
// }

static void draw(struct DrawObj *draw_obj, struct image *img) {
    int vox[3] = {0, 0, 0};
    struct DrawInfo draw_info = {
        .img = img, .vox = vox,
        // .filter = get_filter_algorithm(world),
    };
    draw_component(draw_obj, &draw_info, NULL);
}

static void print_size(char *prefix, struct Memory *memory) {
    size_t size = memory_size(memory);
    if (size < 2 * 1024) {
        fprintf(stderr, "%s: %ldB\n", prefix, size);
    } else if (size < 2 * 1024 * 1024) {
        fprintf(stderr, "%s: %ldkB\n", prefix, size / 1024);
    } else {
        fprintf(stderr, "%s: %ldMB\n", prefix, size / 1024 / 1024);
    }
}

static void log_msg(char severity, char* msg, char* filename, int line, int row) {
    if (filename) {
        fprintf(stderr, "%c %s:%d:%d %s\n", severity, filename, line, row, msg);
    } else {
        fprintf(stderr, "%c %s\n", severity, msg);
    }
}

int main(int argc, char **argv) {
    char *in_filename = argc > 1 ? argv[1] : "-";
    char *out_filename = argc > 2 ? argv[2] : "out.png";

    builder_init();
    builder_texture_init();
    css_eval_start();
    struct Program *program;
    if (!strcmp(in_filename, "-")) {
        program = css_parse_file_as_stream(stdin, log_msg);
    } else {
        program = css_parse_file(in_filename, log_msg);
    }

    if (!program) {
        log_msg('E', "parsing file failed.", NULL, 0, 0);
        return -1;
    }

    print_size("syntax size", program->syntax_memory);

    // css_debug_program(stderr, program);
    struct Rule *world_rule = find_world(program);
    if (!world_rule) {
        log_msg('E', "world rule not found.", NULL, 0, 0);
        return -1;
    }
    struct DrawObj *draw_obj = builder_make(program, world_rule);
    print_size("product size", program->product_memory);

    struct image *img = make_img(world_rule);
    draw(draw_obj, img);

    FILE *fp = strcmp(out_filename, "-") ? fopen(out_filename, "wb") : stdout;
    if (!fp) {
        log_msg('E', "Problem with creating file.", out_filename, 0, 0);
        return -1;
    }

    struct FlatImage *final_img = convert_royal_to_flat(img);
    write_png_file_from_flat_image(fp, final_img);

    fclose(fp);
    builder_stop();
    builder_texture_stop();
    css_eval_stop();
    css_free_program(program);
    destroy_image(img);
    flat_image_destroy(final_img);

    return 0;
}
