#include <stdlib.h>
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
    double *normal;
    struct FlatImage *img_to_draw;
    struct h_poly *left, *right;
};

static inline void _compute_normal(double normal[3], const int voxes[9]) {
    int vox_a[3] = {
        voxes[3 + 0] - voxes[0 + 0],
        voxes[3 + 1] - voxes[0 + 1],
        voxes[3 + 2] - voxes[0 + 2]};
    int vox_b[3] = {
        voxes[6 + 0] - voxes[0 + 0],
        voxes[6 + 1] - voxes[0 + 1],
        voxes[6 + 2] - voxes[0 + 2]};

    normal[0] = vox_a[1] * vox_b[2] - vox_a[2] * vox_b[1];
    normal[1] = vox_a[0] * vox_b[2] - vox_a[2] * vox_b[0];
    normal[2] = vox_a[0] * vox_b[1] - vox_a[1] * vox_b[0];

    double _max = normal[0] > normal[1] ? normal[0] : normal[1];
    double max = _max > normal[2] ? _max : normal[2];

    if (max != 0.0) {
        normal[0] /= max;
        normal[1] /= max;
        normal[2] /= max;
    }
}

static inline void _cpy_h_poly(struct h_poly *a, const struct h_poly *b) {
    a->x = b->x;
    a->y = b->y;
    a->u = b->u;
    a->v = b->v;
    a->zindex = b->zindex;
}

static inline void _add_h_poly_diffy(struct h_poly *a, const struct h_poly_diffy *b) {
    a->y += 1.0;
    a->x += b->x;
    a->u += b->u;
    a->v += b->v;
    a->zindex += b->zindex;
}

static inline void _add_h_poly_diffx(struct h_poly *a, const struct h_poly_diffx *b) {
    a->x += 1.0;
    a->u += b->u;
    a->v += b->v;
    a->zindex += b->zindex;
}

static inline void _diff_h_poly_with_y(
        struct h_poly_diffy *diff,
        const struct h_poly *a,
        const struct h_poly *b) {
    const double diff_y = SUB(a, b, y);
    if (diff_y <= 0.0) {
        diff->x = diff->u = diff->v = diff->zindex = 0;
        return;
    }
    diff->x = SUB(a, b, x) / diff_y;
    diff->u = SUB(a, b, u) / diff_y;
    diff->v = SUB(a, b, v) / diff_y;
    diff->zindex = SUB(a, b, zindex) / diff_y;
}

static inline void _diff_h_poly_with_x(
        struct h_poly_diffx *diff,
        const struct h_poly *a,
        const struct h_poly *b) {
    const double diff_x = SUB(a, b, x);
    if (diff_x <= 0.0) {
        diff->u = diff->v = diff->zindex = 0;
        return;
    }
    diff->u = SUB(a, b, u) / diff_x;
    diff->v = SUB(a, b, v) / diff_x;
    diff->zindex = SUB(a, b, zindex) / diff_x;
}

static inline void _putpixel(
        struct image *img,
        const struct FlatImage *img_to_draw,
        const int img_cor[2],
        const int uv_cor[2],
        const int zindex,
        const double normal[3]) {
    struct rgb color = flat_image_get_pixel(img_to_draw, uv_cor);

    if (color.r == 0xFF && color.g == 0x00 && color.b == 0xFF) {
        // 0xFF00FF color reversed for transparency color
        return;
    }

    // primitive shading
    color.r *= fmin(1.0, 1.0 - normal[0] * 0.35);
    color.g *= fmin(1.0, 1.0 - normal[0] * 0.35);
    color.b *= fmin(1.0, 1.0 - normal[0] * 0.35);

    struct RoyalPixel royal_color = {
        .r=color.r, .g=color.g, .b=color.b,
        .zindex=zindex,
    };

    set_pixel(img, img_cor, royal_color);
}

static inline void _putpixel_with_h_poly(
        struct h_fill_space *helper,
        struct h_poly *point) {
    const int uv_cor[2] = { round(point->u), round(point->v) };
    const int img_cor[2] = { round(point->x), round(point->y) };
    _putpixel(
        helper->img,
        helper->img_to_draw,
        img_cor,
        uv_cor,
        (int)point->zindex,
        helper->normal);
}

static void _fill_space(
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

    for(y_start = left->y; y_start < y_end; y_start += 1.0) {
        _cpy_h_poly(&pointer, left);
        _diff_h_poly_with_x(&pointer_diff, right, left);

        x_start = left->x;
        x_end = right->x;
        for(; x_start < x_end; x_start += 1.0) {
            _putpixel_with_h_poly(helper, &pointer);
            _add_h_poly_diffx(&pointer, &pointer_diff);
        }
        _putpixel_with_h_poly(helper, &pointer);

        _add_h_poly_diffy(left, diff_left);
        _add_h_poly_diffy(right, diff_right);
    }
}

void _projection_poly_isometric(const int *vox, const int *uv, struct h_poly *vec) {
    vec->x = vox[0] - vox[1] + 600.0;
    vec->y = vox[2] + vox[0] * 0.5 + vox[1] * 0.5;

    vec->zindex = vox[1];
    vec->u = uv[0];
    vec->v = uv[1];
}

void _projection_poly_oblique(const int *vox, const int *uv, struct h_poly *vec) {
    // https://en.wikipedia.org/wiki/Oblique_projection
    double s = vox[1] * SCALE_PROJECTION;

    vec->x = floor(vox[0] + s * COS_PROJECTION);
    vec->y = floor(vox[2] + s * SIN_PROJECTION);

    vec->zindex = vox[1];
    vec->u = uv[0];
    vec->v = uv[1];
}

void (*_projection_poly)(const int*, const int*, struct h_poly*) = _projection_poly_oblique;

void draw_poly(
        struct image *img, 
        const struct FlatImage *img_to_draw,
        const int voxes[9], 
        const int uv[6]) {
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

    double normal[3];
    _compute_normal(normal, voxes);

    struct h_fill_space helper = {
        .img=img,
        .normal=normal,
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

void draw_sprite(
        struct image *img,
        const struct FlatImage *img_to_draw,
        const int vox[3],
        const double normal[3]) {
    int uv_cor[2];
    struct h_poly vec;
    _projection_poly(vox, uv_cor, &vec); // TODO - how to avoid uv? new structure, new function?

    const int w = img_to_draw->width;
    const int h = img_to_draw->height;
    const int x_offset = 0; //-w / 2; - TODO better offset
    const int y_offset = 0; //-h / 2;
    const int zindex = vec.zindex;
    const int x = vec.x;
    const int y = vec.y;

    for(uv_cor[0] = 0; uv_cor[0] < w; uv_cor[0]++) {
        for(uv_cor[1] = 0; uv_cor[1] < h; uv_cor[1]++) {
            const int img_cor[2] = {
                uv_cor[0] + x + x_offset,
                uv_cor[1] + y + y_offset,
            };
            _putpixel(img, img_to_draw, img_cor, uv_cor, zindex, normal);
        }
    }
}

static inline void _get_random_vox_in_poly(int vox[3], const int voxes[9]) {
    double rand_a = (rand() % 1024) / 1024.0;
    double rand_b = (rand() % 1024) / 1024.0;

    if (rand_a + rand_b >= 1.0) {
        rand_a = 1.0 - rand_a;
        rand_b = 1.0 - rand_b;
    }

#define COMP_RAND(axis) (\
        (double)voxes[axis + 0] \
        + rand_a * (voxes[axis + 3] - voxes[axis + 0]) \
        + rand_b * (voxes[axis + 6] - voxes[axis + 0]) \
    )
    vox[0] = COMP_RAND(0);
    vox[1] = COMP_RAND(1);
    vox[2] = COMP_RAND(2);
#undef COMP_RAND
}

static inline int _get_area_of_poly(const int voxes[9]) {
#define X 0
#define Y 1
#define Z 2
    const int ab[3] = {
        voxes[X] - voxes[3 + X],
        voxes[Y] - voxes[3 + Y],
        voxes[Z] - voxes[3 + Z],
    };
    const int ac[3] = {
        voxes[X] - voxes[6 + X],
        voxes[Y] - voxes[6 + Y],
        voxes[Z] - voxes[6 + Z],
    };
#define PARTIAL_CROSS(axis_a, axis_b) (ab[axis_a] * ac[axis_b] - ab[axis_b] * ac[axis_a])
    const int yz = PARTIAL_CROSS(Y, Z);
    const int xy = PARTIAL_CROSS(X, Y);
    const int xz = PARTIAL_CROSS(X, Z);
#undef PARTIAL_CROSS

    return sqrt(yz * yz + xz * xz + xy * xy) / 2.0;
#undef X
#undef Y
#undef Z
}

void draw_sprites_in_random_position_in_poly(
        struct image *img,
        const struct FlatImage *img_to_draw,
        const int voxes[3],
        const int density) {
    if (density == 0) return;

    int vox[3];
    double normal[3];
    _compute_normal(normal, voxes);

    const int area = _get_area_of_poly(voxes);
    const int sprite_area = img_to_draw->width * img_to_draw->height;
    const int count = (area * density) / (10 * sprite_area); // IDK - 10 is fine for eyecandy

    int i;
    for(i = 0; i < count; i++) {
        _get_random_vox_in_poly(vox, voxes);
        draw_sprite(img, img_to_draw, vox, normal);
    }
}
