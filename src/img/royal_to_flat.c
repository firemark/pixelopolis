#include "pixelopolis/img/royal_to_flat.h"

#include <math.h>

#include "pixelopolis/img/create.h"
#include "pixelopolis/img/draw.h"

static inline int _abs(int x) { return x > 0 ? x : -x; }

static inline float normal_dist(float a, float b, float c) { return a * a + b * b + c * c; }

// static inline void color_add(struct rgb* color, uint8_t highlight) {
//     color->r = color->r > 255 - highlight ? 255 : color->r + highlight;
//     color->g = color->g > 255 - highlight ? 255 : color->g + highlight;
//     color->b = color->b > 255 - highlight ? 255 : color->b + highlight;
// }

static inline void color_sub(struct rgb* color, uint8_t shadow) {
    color->r = color->r < shadow ? 0 : color->r - shadow;
    color->g = color->g < shadow ? 0 : color->g - shadow;
    color->b = color->b < shadow ? 0 : color->b - shadow;
}

struct FlatImage* convert_royal_to_flat(const struct image* royal) {
    struct FlatImage* img = flat_image_create(royal->width, royal->height);
    if (!img) {
        return NULL;
    }

    size_t size = royal->width * royal->height;
    for (size_t i = 0; i < size; i++) {
        struct RoyalPixel* pixel = &royal->buffer[i];
        struct rgb* color = &img->buffer[i];

        color->r = pixel->r;
        color->g = pixel->g;
        color->b = pixel->b;

        // primitive shading
        float shadow =
            fmax(0.5, fmin(1.0, 1.0 - pixel->x * 0.35 - pixel->y * 0.10 - pixel->z * 0.10));
        color->r *= shadow;
        color->g *= shadow;
        color->b *= shadow;

        // DEBUG NORMAL
        // color->r = 255.0 * (pixel->x * 0.5 + 0.5);
        // color->g = 255.0 * (pixel->y * 0.5 + 0.5);
        // color->b = 255.0 * (pixel->z * 0.5 + 0.5);

        // DEBUG ZINDEX
        // color->r = pixel->zindex;
        // color->g = pixel->zindex;
        // color->b = pixel->zindex;
    }

    for (int y = 1; y < royal->height - 1; y++) {
        for (int x = 1; x < royal->width - 1; x++) {
            int prev_y_cor[2] = {x, y - 1};
            int prev_x_cor[2] = {x - 1, y};
            int next_y_cor[2] = {x, y + 1};
            int next_x_cor[2] = {x + 1, y};
            int cor[2] = {x, y};
            struct RoyalPixel* prev_y_pixel = get_pixel_ptr(royal, prev_y_cor);
            struct RoyalPixel* prev_x_pixel = get_pixel_ptr(royal, prev_x_cor);
            struct RoyalPixel* next_y_pixel = get_pixel_ptr(royal, next_y_cor);
            struct RoyalPixel* next_x_pixel = get_pixel_ptr(royal, next_x_cor);
            struct RoyalPixel* pixel = get_pixel_ptr(royal, cor);
            int diff = 0;
            diff += _abs(pixel->zindex - prev_x_pixel->zindex);
            diff += _abs(pixel->zindex - next_x_pixel->zindex);
            diff += _abs(pixel->zindex - prev_y_pixel->zindex);
            diff += _abs(pixel->zindex - next_y_pixel->zindex);

            float normal_diff_x = 0.0f;
            float normal_diff_y = 0.0f;
            normal_diff_y += normal_dist(      //
                (prev_y_pixel->x - pixel->x),  //
                (prev_y_pixel->y - pixel->y),  //
                (prev_y_pixel->z - pixel->z));
            normal_diff_y += normal_dist(      //
                (next_y_pixel->x - pixel->x),  //
                (next_y_pixel->y - pixel->y),  //
                (next_y_pixel->z - pixel->z));
            normal_diff_x += normal_dist(      //
                (prev_x_pixel->x - pixel->x),  //
                (prev_x_pixel->y - pixel->y),  //
                (prev_x_pixel->z - pixel->z));
            normal_diff_x += normal_dist(      //
                (next_x_pixel->x - pixel->x),  //
                (next_x_pixel->y - pixel->y),  //
                (next_x_pixel->z - pixel->z));

            struct rgb* color = flat_image_get_pixel_ptr(img, cor);
            if (diff > 15) {
                color_sub(color, 64);
            }
            if (normal_diff_y + normal_diff_x > 1.0f) {
                color_sub(color, 32);
            }
        }
    }

    return img;
}
