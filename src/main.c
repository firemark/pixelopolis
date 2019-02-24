#include <basic.h>
#include <stdlib.h>
#include <stdio.h>

#include <img.h>
#include <draw.h>
#include <draw_shape.h>

#include <css.h>

void do_sth2(struct image *img) {
    struct image *img_template = read_png_file("tex.png");
    int vox_up[3] = {64, 64, 64 + 48};
    draw_plane(img, img_template, vox_up, 64, 64, DIRECTION_UP);
    int vox_east[3] = {128, 64, 64};
    draw_plane(img, img_template, vox_east, 64, 48, DIRECTION_EAST);
    int vox_south[3] = {64, 64, 64};
    draw_plane(img, img_template, vox_south, 64, 48, DIRECTION_SOUTH);
}

void do_sth(struct image *img) {
    float x, y;
    float w = img->width;
    float h = img->height;
    for (x=0; x < w; x += 12.35)
    for (y=0; y < h; y += 12.35) {
        struct rgb color = {
            .r=(int)x & 0xff,
            .g=(int)y & 0xff,
            .b=((int)x / 0xff * 64) & 0xff
        };
        float cor[2] = {x, y};
        set_aa_pixel(img, cor, color);
    }

    do_sth2(img);
}

int main(int argc, char **argv) {
    struct image *img = create_black_image(800, 800);
    struct Program *program = css_parse_file("example.css");
    css_debug_program(stderr, program);
    do_sth(img);
    write_png_file("out.png", img);
    return 0;
}
