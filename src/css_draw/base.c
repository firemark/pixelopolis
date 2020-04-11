#include "_css_draw.h"
#include "draw_poly.h"


void css_base_draw_poly_random(struct image* img, const int voxes[9], const struct WallObj* wall) {
    const int density = wall->points_density;
    const struct TexObj* tex = wall->points_tex;
    if (!tex || density <= 0) return;
    const struct FlatImage* sprite = tex->texture;
    if (!sprite) return;
    draw_sprites_in_random_position_in_poly(img, sprite, voxes, density);
}

void css_base_draw_plane(
        struct image* img,
        const struct FlatImage* img_to_draw,
        const int voxes[12],
        const struct WallObj* wall) {
    /* 2---3
     * |B /|
     * | / |
     * |/ A|
     * 0---1
     */
    const int w = img_to_draw->width;
    const int h = img_to_draw->height;
    const int uv[4] = {
        0, 0,
        w, h,
    };

    css_base_draw_plane_with_uv(img, img_to_draw, voxes, uv, wall);
}

void css_base_draw_plane_with_uv(
        struct image* img,
        const struct FlatImage* img_to_draw,
        const int voxes[12],
        const int uv[4],
        const struct WallObj* wall) {
    /* sx,ey  ex,ey
     *    2---3
     *    |B /|
     *    | / |
     *    |/ A|
     *    0---1
     * sx,sy  ex,sy
     */
#define VEC(n) voxes[(n) * 3 + 0], voxes[(n) * 3 + 1], voxes[(n) * 3 + 2]
    const int voxes_a[9] = { VEC(0), VEC(1), VEC(3) };
    const int voxes_b[9] = { VEC(3), VEC(2), VEC(0) };
#undef VEC
    // start point
    const int sx = uv[0];
    const int sy = uv[1];
    // end point
    const int ex = uv[2];
    const int ey = uv[3];

    const int uv_a[6] = {
        sx, sy, // 0
        ex, sy, // 1
        ex, ey, // 3
    };
    const int uv_b[6] = {
        ex, ey, // 3
        sx, ey, // 2
        sx, sy, // 0
    };

    draw_poly(img, img_to_draw, voxes_a, uv_a);
    draw_poly(img, img_to_draw, voxes_b, uv_b);

    css_base_draw_poly_random(img, voxes_a, wall);
    css_base_draw_poly_random(img, voxes_b, wall);
}

void css_base_draw_wide_triangle(
        struct image* img,
        const struct FlatImage* img_to_draw,
        const int voxes[9],
        const struct WallObj* wall) {
    /*     2
     *    / \
     *   /   \
     *  /     \
     * 0-------1
     */
    const int h = img_to_draw->height;
    const int w = img_to_draw->width;
    const int wh = w / 2;
    const int uv[6] = {
        0, 0, // 0
        w, 0, // 1
        wh, h // 2
    };
    draw_poly(img, img_to_draw, voxes, uv);
    css_base_draw_poly_random(img, voxes, wall);
}
