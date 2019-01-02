#include <basic.h>
#include <stdlib.h>

#include <img.h>
#include <draw.h>

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
}

int main(int argc, char **argv) {
    struct image *img = create_black_image(800, 800);
    do_sth(img);
    write_png_file("out.png", img);
    return 0;
}
