#include "pixelopolis/draw.h"

#include <math.h>

#include "pixelopolis/basic.h"

#define _GET_INDEX(img, cor) ((img->height - 1 - cor[1]) * img->width + cor[0])
#define _GUARD_INDEX(index, img)                          \
    if (index < 0 || index >= img->width * img->height) { \
        index = 0;                                        \
    }
#define _GUARD_COR(cor, img)               \
    {                                      \
        if (cor[0] < 0) return;            \
        if (cor[0] >= img->width) return;  \
        if (cor[1] < 0) return;            \
        if (cor[1] >= img->height) return; \
    }

#define _GET_PIXEL(img, cor) \
    int index = _GET_INDEX(img, cor); \
    _GUARD_INDEX(index, img); \
    return img->buffer[index]

#define _SET_PIXEL(img, cor, color) \
    _GUARD_COR(cor, img); \
    const int index = _GET_INDEX(img, cor); \
    img->buffer[index] = color

struct RoyalPixel get_pixel(const struct image* img, const int cor[2]) {
    _GET_PIXEL(img, cor);
}

struct rgb flat_image_get_pixel(const struct FlatImage* img, const int cor[2]) {
    _GET_PIXEL(img, cor);
}

unsigned char one_chan_image_get_pixel(const struct OneChanImage* img, const int cor[2]) {
    _GET_PIXEL(img, cor);
}

struct xyz float_image_get_pixel(const struct FloatImage* img, const int cor[2]) {
    _GET_PIXEL(img, cor);
}

struct rgb flat_image_get_aa_pixel(const struct FlatImage* img, const double cor[2]) {
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

#define M(c, x, y) c* x* y
#define MIX(x) \
    M(plu.x, dx, dy) + M(pld.x, dx, inv_dy) + M(pru.x, inv_dx, dy) + M(prd.x, inv_dx, inv_dy)

    struct rgb p = {
        .r = MIX(r),
        .g = MIX(g),
        .b = MIX(b),
    };

#undef MIX
#undef M

    return p;
}

static void _set_pixel(struct image* img, const int cor[2], const struct RoyalPixel color) {
    const int index = _GET_INDEX(img, cor);
    img->buffer[index] = color;
}

void set_pixel(struct image* img, const int cor[2], const struct RoyalPixel color) {
    _GUARD_COR(cor, img);
    const int index = _GET_INDEX(img, cor);
    const struct RoyalPixel old_color = img->buffer[index];
    if (old_color.zindex >= color.zindex) {
        img->buffer[index] = color;
    }
}

void flat_image_set_pixel(const struct FlatImage* img, const int cor[2], const struct rgb color) {
    _SET_PIXEL(img, cor, color);
}

void float_image_set_pixel(const struct FloatImage* img, const int cor[2], const struct xyz color) {
    _SET_PIXEL(img, cor, color);
}

void one_chan_image_set_pixel(const struct OneChanImage* img, const int cor[2], const unsigned char color) {
    _SET_PIXEL(img, cor, color);
}

static inline struct RoyalPixel _mix_color(const struct RoyalPixel a, const struct RoyalPixel b,
                                           const double ratio) {
    const float inv_ratio = 1 - ratio;
    struct RoyalPixel c;

    c.r = a.r * ratio + b.r * inv_ratio;
    c.g = a.g * ratio + b.g * inv_ratio;
    c.b = a.b * ratio + b.b * inv_ratio;

    return c;
}

void set_aa_pixel(struct image* img, const double cor[2], const struct RoyalPixel color) {
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
