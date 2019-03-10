#include <basic.h>
#include <draw.h>
#include <math.h>


int _get_index(struct image* img, int cor[2]) {
    int x = cor[0];
    int y = img->height - 1 - cor[1];
    return y * img->width + x;
}

struct rgb get_pixel(struct image* img, int cor[2]) {
    int index = _get_index(img, cor);
    return img->buffer[index];
}

void _set_pixel(struct image* img, int cor[2], struct rgb color) {
    int index = _get_index(img, cor);
    img->buffer[index] = color;
}

void set_pixel(struct image* img, int cor[2], struct rgb color) {
    int index = _get_index(img, cor);
    struct rgb old_color = img->buffer[index];
    if (old_color.zindex >= color.zindex) {
        img->buffer[index] = color;
    }
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

    int l = (int)x;
    int r = l + 1;
    int u = (int)y;
    int d = u + 1;

    float dx = x - floor(x);
    float dy = y - floor(y);
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

    if (pixel_lu.zindex >= color.zindex) {
        struct rgb mix_pixel_lu = _mix_color(pixel_lu, color, dx * dy);
        mix_pixel_lu.zindex = color.zindex;
        _set_pixel(img, lu, mix_pixel_lu);
    }

    if (pixel_ld.zindex >= color.zindex) {
        struct rgb mix_pixel_ld = _mix_color(pixel_ld, color, dx * inv_dy);
        mix_pixel_ld.zindex = color.zindex;
        _set_pixel(img, ld, mix_pixel_ld);
    }

    if (pixel_ru.zindex >= color.zindex) {
        struct rgb mix_pixel_ru = _mix_color(pixel_ru, color, inv_dx * dy);
        mix_pixel_ru.zindex = color.zindex;
        _set_pixel(img, ru, mix_pixel_ru);
    }

    if (pixel_rd.zindex >= color.zindex) {
        struct rgb mix_pixel_rd = _mix_color(pixel_rd, color, inv_dx * inv_dy);
        mix_pixel_rd.zindex = color.zindex;
        _set_pixel(img, rd, mix_pixel_rd);
    }
}
