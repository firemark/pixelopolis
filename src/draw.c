#include <basic.h>
#include <draw.h>
#include <math.h>


int _get_index(struct image* img, int cor[2]) {
    int x = cor[0];
    int y = img->height - 1 - cor[1];
    return y * img->width + x;
}

int _flat_image_get_index(struct FlatImage* img, int cor[2]) {
    int x = cor[0];
    int y = img->height - 1 - cor[1];
    return y * img->width + x;
}

struct RoyalPixel get_pixel(struct image* img, int cor[2]) {
    int index = _get_index(img, cor);
    if (index < 0 || index >= img->width * img->height) {
        index = 0;
    }
    return img->buffer[index];
}

struct rgb flat_image_get_pixel(struct FlatImage* img, int cor[2]) {
    int index = _flat_image_get_index(img, cor);
    if (index < 0 || index >= img->width * img->height) {
        index = 0;
    }
    return img->buffer[index];
}

struct rgb flat_image_get_aa_pixel(struct FlatImage* img, float cor[2]) {
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

    struct rgb plu = flat_image_get_pixel(img, lu);
    struct rgb pld = flat_image_get_pixel(img, ld);
    struct rgb pru = flat_image_get_pixel(img, ru);
    struct rgb prd = flat_image_get_pixel(img, rd);

#define M(c, x, y) c * x * y
#define MIX(x) M(plu. x, dx, dy) + M(pld. x, dx, inv_dy) + M(pru. x, inv_dx, dy) + M(prd. x, inv_dx, inv_dy)

    struct rgb p = {
        .r=MIX(r),
        .g=MIX(g),
        .b=MIX(b),
    };

#undef MIX
#undef M

    return p;
}

void _set_pixel(struct image* img, int cor[2], struct RoyalPixel color) {
    int index = _get_index(img, cor);
    if (index >= img->width * img->height) return;
    img->buffer[index] = color;
}

void set_pixel(struct image* img, int cor[2], struct RoyalPixel color) {
    int index = _get_index(img, cor);
    struct RoyalPixel old_color = img->buffer[index];
    if (old_color.zindex >= color.zindex) {
        img->buffer[index] = color;
    }
}

struct RoyalPixel _mix_color(struct RoyalPixel a, struct RoyalPixel b, float ratio) {
    struct RoyalPixel c;
    float inv_ratio = 1 - ratio;

    c.r = a.r * ratio + b.r * inv_ratio;
    c.g = a.g * ratio + b.g * inv_ratio;
    c.b = a.b * ratio + b.b * inv_ratio;

    return c;
}

void set_aa_pixel(struct image* img, float cor[2], struct RoyalPixel color) {
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

    struct RoyalPixel pixel_lu = get_pixel(img, lu);
    struct RoyalPixel pixel_ld = get_pixel(img, ld);
    struct RoyalPixel pixel_ru = get_pixel(img, ru);
    struct RoyalPixel pixel_rd = get_pixel(img, rd);

    if (pixel_lu.zindex >= color.zindex) {
        struct RoyalPixel mix_pixel_lu = _mix_color(pixel_lu, color, dx * dy);
        mix_pixel_lu.zindex = color.zindex;
        _set_pixel(img, lu, mix_pixel_lu);
    }

    if (pixel_ld.zindex >= color.zindex) {
        struct RoyalPixel mix_pixel_ld = _mix_color(pixel_ld, color, dx * inv_dy);
        mix_pixel_ld.zindex = color.zindex;
        _set_pixel(img, ld, mix_pixel_ld);
    }

    if (pixel_ru.zindex >= color.zindex) {
        struct RoyalPixel mix_pixel_ru = _mix_color(pixel_ru, color, inv_dx * dy);
        mix_pixel_ru.zindex = color.zindex;
        _set_pixel(img, ru, mix_pixel_ru);
    }

    if (pixel_rd.zindex >= color.zindex) {
        struct RoyalPixel mix_pixel_rd = _mix_color(pixel_rd, color, inv_dx * inv_dy);
        mix_pixel_rd.zindex = color.zindex;
        _set_pixel(img, rd, mix_pixel_rd);
    }
}
