#include <basic.h>
#include <draw.h>
#include <math.h>


int _get_index(struct image* img, int cor[2]) {
    return cor[1] * img->width + cor[0];
}

struct rgb get_pixel(struct image* img, int cor[2]) {
    int index = _get_index(img, cor);
    return img->buffer[index];
}

void set_pixel(struct image* img, int cor[2], struct rgb color) {
    int index = _get_index(img, cor);
    img->buffer[index] = color;
}

struct rgb _mix_color(struct rgb a, struct rgb b, float ratio) {
    struct rgb c;
    float inv_ratio = 1 - ratio;

    c.r = a.r * ratio + b.r * inv_ratio;
    c.g = a.g * ratio + b.g * inv_ratio;
    c.b = a.b * ratio + b.b * inv_ratio;
    return c;
}

void set_aa_pixel(struct image* img, float cor[2], struct rgb color) {
    // lu - left up; ld - left down
    // ru - right up; rd - right down
    float x = cor[0];
    float y = cor[1];
    float floor_x = floor(x);
    float floor_y = floor(y);

    int l = (int)x;
    int r = l + 1;
    int u = (int)y;
    int d = u + 1;

    float dx = x - floor_x;
    float dy = y - floor_y;
    float inv_dx = 1.0 - dx;
    float inv_dy = 1.0 - dy;

    int lu[2] = {l, u};
    int ld[2] = {l, d};
    int ru[2] = {r, u};
    int rd[2] = {r, d};

    struct rgb pixel_lu = get_pixel(img, lu);
    struct rgb pixel_ld = get_pixel(img, ld);
    struct rgb pixel_ru = get_pixel(img, ru);
    struct rgb pixel_rd = get_pixel(img, rd);

    set_pixel(img, lu, _mix_color(pixel_lu, color, dx * dy));
    set_pixel(img, ld, _mix_color(pixel_ld, color, dx * inv_dy));
    set_pixel(img, ru, _mix_color(pixel_ru, color, inv_dx * dy));
    set_pixel(img, rd, _mix_color(pixel_rd, color, inv_dx * inv_dy));
}
