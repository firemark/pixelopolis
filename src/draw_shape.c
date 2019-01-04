#include <stdlib.h>

#include <draw_shape.h>
#include <draw.h>

#define SIN_PROJECTION 0.7071067811865476
#define COS_PROJECTION 0.7071067811865476
#define SCALE_PROJECTION 0.6666666666666666

float* _projection(int vox[3]) {
    // https://en.wikipedia.org/wiki/Oblique_projection
    int s = vox[1] * SCALE_PROJECTION;
    float x = vox[0] + s * COS_PROJECTION;
    float y = vox[2] + s * SIN_PROJECTION;
    float *cor = malloc(2 * sizeof(float));

    cor[0] = x;
    cor[1] = y;

    return cor;
}


void draw_plane(
        struct image *img, struct image *img_to_draw,
        int vox[3],
        int width, int height,
        enum direction dir) {
    int w, h;
    for(w = 0; w < width; w++)
    for(h = 0; h < height; h++) {
        int new_vox[3];
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
        int tmp_cor[2] = {w % img_to_draw->width, h % img_to_draw->height};
        struct rgb color = get_pixel(img_to_draw, tmp_cor);
        float* img_cor = _projection(new_vox);
        set_aa_pixel(img, img_cor, color);
        free(img_cor);
    }
}

