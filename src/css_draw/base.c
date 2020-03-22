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
    int uv[4] = {
        0, 0,
        w, h,
    };

    css_base_draw_plane_with_uv(img, img_to_draw, voxes, uv);
}

void css_base_draw_plane_with_uv(struct image* img, struct FlatImage* img_to_draw, int voxes[12], int uv[4]) {
    /* sx,ey  ex,ey
     *    2---3
     *    |B /|
     *    | / |
     *    |/ A|
     *    0---1 
     * sx,sy  ex,sy
     */
#define VEC(n) voxes[(n) * 3 + 0], voxes[(n) * 3 + 1], voxes[(n) * 3 + 2]
    int voxes_a[9] = { VEC(0), VEC(1), VEC(3) };
    int voxes_b[9] = { VEC(3), VEC(2), VEC(0) };
#undef VEC
    // start point
    int sx = uv[0];
    int sy = uv[1];
    // end point
    int ex = uv[2];
    int ey = uv[3];

    int uv_a[6] = {
        sx, sy, // 0
        ex, sy, // 1
        ex, ey, // 3
    };
    int uv_b[6] = {
        ex, ey, // 3
        sx, ey, // 2
        sx, sy, // 0
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
