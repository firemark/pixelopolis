#include <stdio.h>
#include <math.h>

#include "draw_poly.h"
#include "draw.h"

#define SIN_PROJECTION 0.7071067811865476
#define COS_PROJECTION 0.7071067811865476
#define SCALE_PROJECTION 0.6666666666666666

#define SUB(a, b, val) ((a) -> val - (b) -> val)


struct h_poly {
    double x, y;
    double u, v;
    double zindex;
};

struct h_poly_diffy {
    double x;
    double u, v;
    double zindex;
};

struct h_poly_diffx {
    double u, v;
    double zindex;
};

struct h_fill_space {
    struct image *img; 
    struct FlatImage *img_to_draw;
    struct h_poly *left, *right;
};

void _compute_normal(int normal[3], int voxes[9]) {
    // todo
    int vox_a[3] = {
        voxes[3 + 0] - voxes[0 + 0],
        voxes[3 + 1] - voxes[0 + 1],
        voxes[3 + 2] - voxes[0 + 2]};
    int vox_b[3] = {
        voxes[6 + 0] - voxes[0 + 0],
        voxes[6 + 1] - voxes[0 + 1],
        voxes[6 + 2] - voxes[0 + 2]};

    normal[0] = vox_a[2] * vox_b[3] - vox_a[3] * vox_b[2];
    normal[1] = vox_a[1] * vox_b[3] - vox_a[3] * vox_b[1];
    normal[2] = vox_a[1] * vox_b[2] - vox_a[2] * vox_b[1];
}

void _cpy_h_poly(struct h_poly *a, struct h_poly *b) {
    a->x = b->x;
    a->y = b->y;
    a->u = b->u;
    a->v = b->v;
    a->zindex = b->zindex;
}

void _add_h_poly_diffy(struct h_poly *a, struct h_poly_diffy *b) {
    a->y += 1.0;
    a->x += b->x;
    a->u += b->u;
    a->v += b->v;
    a->zindex += b->zindex;
}

void _add_h_poly_diffx(struct h_poly *a, struct h_poly_diffx *b) {
    a->x += 1.0;
    a->u += b->u;
    a->v += b->v;
    a->zindex += b->zindex;
}

void _diff_h_poly_with_y(
        struct h_poly_diffy *diff,
        struct h_poly *a,
        struct h_poly *b) {
    double diff_y = SUB(a, b, y);
    if (diff_y <= 0.0) {
        diff->x = diff->u = diff->v = diff->zindex = 0;
        return;
    }
    diff->x = SUB(a, b, x) / diff_y;
    diff->u = SUB(a, b, u) / diff_y;
    diff->v = SUB(a, b, v) / diff_y;
    diff->zindex = SUB(a, b, zindex) / diff_y;
}

void _diff_h_poly_with_x(
        struct h_poly_diffx *diff,
        struct h_poly *a,
        struct h_poly *b) {
    double diff_x = SUB(a, b, x);
    if (diff_x <= 0.0) {
        diff->u = diff->v = diff->zindex = 0;
        return;
    }
    diff->u = round(SUB(a, b, u) / diff_x);
    diff->v = round(SUB(a, b, v) / diff_x);
    diff->zindex = round(SUB(a, b, zindex) / diff_x);
}

void _putpixel(
        struct image *img, 
        struct FlatImage *img_to_draw,
        struct h_poly *point) {
    int tmp_cor[2] = { round(point->u), round(point->v) };
    int img_cor[2] = { round(point->x), round(point->y) };
    struct rgb color = flat_image_get_pixel(img_to_draw, tmp_cor);
    struct RoyalPixel royal_color = {
        .r=color.r, .g=color.g, .b=color.b,
        .zindex=(int)point->zindex
    };

    set_pixel(img, img_cor, royal_color);
}

void _fill_space(
        struct h_fill_space *helper,
        struct h_poly_diffy *diff_left, 
        struct h_poly_diffy *diff_right,
        double y_end) {
    double y_start, x_start, x_end;
    struct h_poly pointer;
    struct h_poly_diffx pointer_diff;
    struct h_poly *left = helper->left;
    struct h_poly *right = helper->right;

    if (left->x > right->x) { // swap right with left if need
        struct h_poly *temp;
        struct h_poly_diffy *diff_temp;
        temp = left;
        left = right;
        right = temp;

        diff_temp = diff_left;
        diff_left = diff_right;
        diff_right = diff_temp;
    }

    for(y_start = left->y; y_start <= y_end; y_start += 1.0) {
        _cpy_h_poly(&pointer, left);
        _diff_h_poly_with_x(&pointer_diff, right, left);

        x_start = left->x;
        x_end = right->x;
        for(; x_start <= x_end; x_start += 1.0) {
            _putpixel(helper->img, helper->img_to_draw, &pointer);
            _add_h_poly_diffx(&pointer, &pointer_diff);
        }

        _add_h_poly_diffy(left, diff_left);
        _add_h_poly_diffy(right, diff_right);
    }
}

void _projection_poly(int *vox, int *uv, struct h_poly *vec) {
    // https://en.wikipedia.org/wiki/Oblique_projection
    double s = vox[1] * SCALE_PROJECTION;

    vec->x = vox[0] + s * COS_PROJECTION;
    vec->y = vox[2] + s * SIN_PROJECTION;
    vec->zindex = vox[1];
    vec->u = uv[0];
    vec->v = uv[1];
}

void draw_poly(
        struct image *img, struct FlatImage *img_to_draw,
        int voxes[9], int uv[6]) {
    struct h_poly at, bt, ct;
    struct h_poly *a, *b, *c;

    a = &at; // fuck malloc, use stock!
    b = &bt;
    c = &ct;

    // set projection of vectors
    _projection_poly(&voxes[0], &uv[0], a);
    _projection_poly(&voxes[3], &uv[2], b);
    _projection_poly(&voxes[6], &uv[4], c);

    // sorting
#define SWAP(a, b) { temp = a; a = b; b = temp; }
    struct h_poly *temp;
    if (a->y > b->y) SWAP(a, b);
    if (a->y > c->y) SWAP(a, c);
    if (b->y > c->y) SWAP(b, c);
#undef SWAP   

    // compute diffs to linear interpolation
    struct h_poly_diffy diff_ba, diff_ca, diff_cb;
    _diff_h_poly_with_y(&diff_ba, b, a);
    _diff_h_poly_with_y(&diff_ca, c, a);
    _diff_h_poly_with_y(&diff_cb, c, b);

    // set left and right pointer to fill lines
    struct h_poly left, right;
    _cpy_h_poly(&left, a);
    _cpy_h_poly(&right, a);

    struct h_fill_space helper = {
        .img=img,
        .img_to_draw=img_to_draw,
        .left=&left,
        .right=&right,
    };

    if (diff_ba.x > diff_ca.x) {
        _fill_space(&helper, &diff_ca, &diff_ba, b->y);
        _cpy_h_poly(helper.right, b);
        _fill_space(&helper, &diff_ca, &diff_cb, c->y);
    } else {
        _fill_space(&helper, &diff_ba, &diff_ca, b->y);
        _cpy_h_poly(helper.left, b);
        _fill_space(&helper, &diff_cb, &diff_ca, c->y);
    }
};
