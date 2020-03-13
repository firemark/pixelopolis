#include "_css_draw.h"
#include "draw_poly.h"

void css_base_draw_plane(struct image* img, struct FlatImage* img_to_draw, int voxes[12]) {
    /* 2---3
     * |B /|
     * | / |
     * |/ A|
     * 0---1 
     */
    int w = img_to_draw->width;
    int h = img_to_draw->height;
#define VEC(n) voxes[(n) * 3 + 0], voxes[(n) * 3 + 1], voxes[(n) * 3 + 2]
    int voxes_a[9] = { VEC(0), VEC(1), VEC(3) };
    int voxes_b[9] = { VEC(3), VEC(2), VEC(0) };
#undef VEC
    int uv_a[6] = {
        0, 0, // 0
        w, 0, // 1
        w, h, // 3
    };
    int uv_b[6] = {
        w, h, // 3
        0, h, // 2
        0, 0, // 0
    };

    draw_poly(img, img_to_draw, voxes_a, uv_a);
    draw_poly(img, img_to_draw, voxes_b, uv_b);
}

void css_base_draw_wide_triangle(struct image* img, struct FlatImage* img_to_draw, int voxes[9]) {
    /*     2
     *    / \
     *   /   \
     *  /     \
     * 0-------1
     */
    int h = img_to_draw->height;
    int w = img_to_draw->width;
    int wh = w / 2;
    int uv[6] = {
        0, 0, // 0
        w, 0, // 1
        wh, h // 2
    }; 
    draw_poly(img, img_to_draw, voxes, uv);
}
