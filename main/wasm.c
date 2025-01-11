#include <emscripten.h>
#include <stdlib.h>

#include "pixelopolis/css.h"
#include "pixelopolis/css/eval.h"
#include "pixelopolis/css/func.h"
#include "pixelopolis/draw_builder_common.h"
#include "pixelopolis/draw_builder_texture.h"
#include "pixelopolis/draw_builder_world.h"
#include "pixelopolis/img.h"
#include "pixelopolis/img/draw.h"
#include "pixelopolis/img/royal_to_flat.h"
#include "pixelopolis/world_draw.h"

static struct Program *_program;
static struct DrawObj *_obj;

int main() {
    _program = NULL;
    _obj = NULL;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int init() {
    _program = NULL;
    _obj = NULL;
    builder_init();
    builder_texture_init();
    css_eval_start();
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int stop() {
    if (_program) {
        css_free_program(_program);
    }
    _program = NULL;
    _obj = NULL;
    builder_stop();
    builder_texture_stop();
    css_eval_stop();
    return 0;
}

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

static void copy_to_canvas(struct FlatImage *img) {
    uint8_t *buffer = malloc(img->width * img->height * 4);
    uint8_t *ptr = buffer;
    for(size_t y = 0; y < img->height; y++) {
        for(size_t x = 0; x < img->width; x++) {
            int cor[2] = {x, img->height - 1 - y};
            struct rgb color = flat_image_get_pixel(img, cor);
            ptr[0] = color.r;
            ptr[1] = color.g;
            ptr[2] = color.b;
            ptr[3] = 0xFF;
            ptr += 4;
        }
    }
    EM_ASM_(
        {
            let data = Module.HEAPU8.slice($0, $0 + $1 * $2 * 4);
            Module['canvas'].width = $1;
            Module['canvas'].height = $2;
            let context = Module['canvas'].getContext('2d');
            let imageData = context.getImageData(0, 0, $1, $2);
            imageData.data.set(data);
            context.putImageData(imageData, 0, 0);
        },
        buffer, img->width, img->height);
    free(buffer);
}

EMSCRIPTEN_KEEPALIVE
int build(char *code) {
    if (_program) {
        css_free_program(_program);
    }
    _program = css_parse_file_as_string(code);
    free(code);

    struct Rule *world_rule = find_world(_program);
    if (!world_rule) {
        fprintf(stderr, "world rule not found! :(\n");
        return -1;
    }

    _obj = builder_make(_program, world_rule);
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int draw(int x, int y, int z) {
    if (!_program) {
        return -1;
    }
    if (!_obj) {
        return -1;
    }
    struct Rule *world_rule = find_world(_program);
    if (!world_rule) {
        return -1;
    }
    struct image *img = make_img(world_rule);
    int vox[3] = {x, y, z};
    struct DrawInfo draw_info = {.img = img, .vox = vox};
    draw_component(_obj, &draw_info, NULL);

    struct FlatImage *final_img = convert_royal_to_flat(img);
    copy_to_canvas(final_img);

    flat_image_destroy(final_img);
    destroy_image(img);
    return 0;
}