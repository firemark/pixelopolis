#include <draw_shape.h>
#include <draw.h>


void draw_plane(struct image *img, struct image *img_to_draw, int cor[2], int width, int height) {
    int img_x = cor[0];
    int img_y = cor[1];

    int x, y;

    for(x = 0; x < width; x++)
    for(y = 0; y < height; y++) {
        int img_cor[2] = {img_x + x, img_y + y};
        int tmp_cor[2] = {x % img_to_draw->width, y % img_to_draw->height};
        struct rgb color = get_pixel(img_to_draw, tmp_cor);
        set_pixel(img, img_cor, color);
    }
}

