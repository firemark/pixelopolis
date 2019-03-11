#include <stdlib.h>

#include <draw_shape.h>
#include <draw.h>

#define SIN_PROJECTION 0.7071067811865476
#define COS_PROJECTION 0.7071067811865476
#define SCALE_PROJECTION 0.6666666666666666

float* _projection(int vox[3]) {
    // https://en.wikipedia.org/wiki/Oblique_projection
    int s = vox[1] * SCALE_PROJECTION;
    float *cor = malloc(2 * sizeof(float));

    cor[0] = vox[0] + s * COS_PROJECTION;
    cor[1] = vox[2] + s * SIN_PROJECTION;

    return cor;
}

void _default_vox_transform(enum direction dir, int w, int h, int *vox, int *new_vox) {
    switch (dir) {
        case DIRECTION_UP: 
            new_vox[0] = vox[0] + w;
            new_vox[1] = vox[1] + h;
            new_vox[2] = vox[2];
            break;
        case DIRECTION_EAST:
            new_vox[0] = vox[0];
            new_vox[1] = vox[1] + w;
            new_vox[2] = vox[2] + h;
            break;
        case DIRECTION_SOUTH:
            new_vox[0] = vox[0] + w;
            new_vox[1] = vox[1];
            new_vox[2] = vox[2] + h;
            break;
    }
}

void _with_height_vox_transform(enum direction dir, int w, int d, int h, int* vox, int *new_vox) {
    switch (dir) {
        case DIRECTION_EAST:
            new_vox[0] = vox[0] + w;
            new_vox[1] = vox[1] + d;
            new_vox[2] = vox[2] + h;
            break;
        case DIRECTION_SOUTH:
            new_vox[0] = vox[0] + d;
            new_vox[1] = vox[1] + w;
            new_vox[2] = vox[2] + h;
            break;
    }
}

void _draw(
        struct image *img, struct image *img_to_draw,
        int vox[3], int w, int h) {
    int tmp_cor[2] = {w % img_to_draw->width, h % img_to_draw->height};
    struct rgb color = get_pixel(img_to_draw, tmp_cor);
    color.zindex = vox[1];

    float* img_cor = _projection(vox);
    set_aa_pixel(img, img_cor, color);
    free(img_cor);
}

int _get_height(struct DrawArgs *args) {
    return args->height < args->max_height ? args->height : args->max_height;
}

void draw_plane(struct DrawArgs *args) {
    int w, h;
    int new_vox[3];
    int width = args->width;
    int height = _get_height(args);
    for(w = 0; w < width; w++)
    for(h = args->start_height; h < height; h++) {
        _default_vox_transform(args->dir, w, h, args->vox, new_vox);
        _draw(args->img, args->img_to_draw, new_vox, w, h);
    }
}

void draw_triangle(struct DrawArgs *args) {
    int w, h;
    int new_vox[3];
    int width = args->width;
    int height = _get_height(args);
    for(h = args->start_height; h < height; h++) {
        int min_span = width * h / args->max_height / 2;
        int max_span = width - min_span;
        for(w = min_span; w < max_span; w++) {
            _default_vox_transform(args->dir, w, h, args->vox, new_vox);
            _draw(args->img, args->img_to_draw, new_vox, w, h);
        }
    }
}

void draw_plane_with_height(struct DrawArgs *args, int zheight) {
    int w, d;
    int new_vox[3];
    int width = args->width;
    int height = _get_height(args);
    for(w = 0; w < width; w++) {
        char isreverse = w >= width / 2;
        for(d = 0; d < height; d++) {
            int h = isreverse ? width - w - 1 : w;
            h *= 2 * zheight / width;
            _with_height_vox_transform(args->dir, w, d, h, args->vox, new_vox);
            _draw(args->img, args->img_to_draw, new_vox, w, d);
        }
    }
}

