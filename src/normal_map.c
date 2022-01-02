#include <stdlib.h>
#include <stdint.h>

#include <img/create.h>
#include <draw.h>

const float COS_ATAN[255] = {0}; // function cos(atan2(1, diff_x))

static inline uint8_t _normalize(float value) {
    return 128.0f * value + 127.0f;
}

static inline uint8_t _make_last_x(struct OneChanImage* bump_map) {
    int cor[2] = {bump_map->width - 1, 0};
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

static inline struct rgb _compute_normal(int diff_x, int diff_y) {
    // https://en.wikipedia.org/wiki/Inverse_trigonometric_functions
    // atan(-x) = -atan(x)
    // cos(-x) = cos(x)
    // cos(atan(-x)) = cos(-atan(x)) = cos(atan(x))
    float cos_x = COS_ATAN[_abs(diff_x)];
    float cos_y = COS_ATAN[_abs(diff_y)];
    // cos(atan(x)) = 1 / sqrt(1 + x²)
    // sin(atan(x)) = x / sqrt(1 + x²) = x * cos(atan(x))
    float sin_x = diff_x * cos_x;
    float sin_y = diff_y * cos_y;
    struct rgb color = {
        .r=_normalize(sin_x),          // x axis
        .g=_normalize(sin_y),          // y axis
        .b=_normalize(cos_x * cos_y)}; // z axis
    return color;
}

struct FlatImage* transform_bump_to_normal_map(struct OneChanImage* bump_map) {
    uint8_t last_x = _make_last_x(bump_map);
    uint8_t* last_y = _make_last_y(bump_map);
    int width = bump_map->width;
    int height = bump_map->height;
    struct FlatImage* normal_map = flat_image_create(width, height);
    int x_index;
    int y_index;

    for(y_index = 0; y_index < height; y_index++) {
        for(x_index = 0; x_index < width; x_index++) {
            int cor[2] = {x_index, y_index};
            int value = one_chan_image_get_pixel(bump_map, cor);
            int diff_x = value - last_x;
            int diff_y = value - last_y[x_index];
            struct rgb normal = _compute_normal(diff_x, diff_y);
            flat_image_set_pixel(normal_map, cor, normal);
            last_x = value;
            last_y[x_index] = value;
        }
    }

    free(last_y);
    return normal_map;
}
