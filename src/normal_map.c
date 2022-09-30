#include <stdlib.h>
#include <stdint.h>

#include "pixelopolis/img/create.h"
#include "pixelopolis/draw.h"
#include "pixelopolis/_cos_sin_atan.h"


static inline uint8_t _normalize(float value) {
    return 255.0f * (value * 0.5f + 0.5f);
}

static inline uint8_t _make_last_x(struct OneChanImage* bump_map, int y_index) {
    int cor[2] = {bump_map->width - 1, y_index};
    return one_chan_image_get_pixel(bump_map, cor);
}

static inline uint8_t* _make_last_y(struct OneChanImage* bump_map) {
    uint8_t* last_y = malloc(sizeof(uint8_t) * bump_map->height);
    int x_index;
    int y_index = bump_map->height - 1;
    for(x_index = 0; x_index < bump_map->width; x_index++) {
        int cor[2] = {x_index, y_index};
        last_y[x_index] = one_chan_image_get_pixel(bump_map, cor);
    }
    return last_y;
}

static inline uint8_t _abs(int value) {
    return value > 0 ? value : -value;
}

static inline float _sgn(int value) {
    return value > 0 ? -1.0 : 1.0;
}

static inline struct xyz _compute_normal(int diff_x, int diff_y) {
    uint8_t abs_x = _abs(diff_x);
    uint8_t abs_y = _abs(diff_y);
    // https://en.wikipedia.org/wiki/Inverse_trigonometric_functions
    // atan(-x) = -atan(x)
    // cos(-x) = cos(x)
    // cos(atan(-x)) = cos(-atan(x)) = cos(atan(x))
    float cos_x = COS_ATAN[abs_x];
    float cos_y = COS_ATAN[abs_y];
    // sin(-x) = -sin(x)
    // sin(atan(-x)) = sin(-atan(x)) = -sin(atan(x))
    float sin_x = _sgn(diff_x) * SIN_ATAN[abs_x];
    float sin_y = _sgn(diff_y) * SIN_ATAN[abs_y];
    struct xyz normal = {
        .x=sin_x,
        .y=sin_y,
        .z=cos_x * cos_y
    };
    return normal;
}

struct FloatImage* transform_bump_to_normal_map(struct OneChanImage* bump_map) {
    uint8_t* last_y = _make_last_y(bump_map);
    int width = bump_map->width;
    int height = bump_map->height;
    struct FloatImage* normal_map = float_image_create(width, height);
    int x_index;
    int y_index;

    for(y_index = 0; y_index < height; y_index++) {
        uint8_t last_x = _make_last_x(bump_map, y_index);
        for(x_index = 0; x_index < width; x_index++) {
            int cor[2] = {x_index, y_index};
            int value = one_chan_image_get_pixel(bump_map, cor);
            int diff_x = value - last_x;
            int diff_y = value - last_y[x_index];
            struct xyz normal = _compute_normal(diff_x, diff_y);
            float_image_set_pixel(normal_map, cor, normal);
            last_x = value;
            last_y[x_index] = value;
        }
    }

    free(last_y);
    return normal_map;
}

struct FlatImage* tranform_normal_map_to_rgb_map(struct FloatImage* normal_map) {
    int width = normal_map->width;
    int height = normal_map->height;
    struct FlatImage* rgb_map = flat_image_create(width, height);
    int x_index;
    int y_index;

    for(y_index = 0; y_index < height; y_index++) {
        for(x_index = 0; x_index < width; x_index++) {
            int cor[2] = {x_index, y_index};
            struct xyz normal = float_image_get_pixel(normal_map, cor);
            struct rgb color = {
                .r=_normalize(normal.x),
                .g=_normalize(normal.y),
                .b=_normalize(normal.z)
            };
            flat_image_set_pixel(rgb_map, cor, color);
        }
    }

    return rgb_map;
}
