#include "pixelopolis/img/royal_to_flat.h"

#include <math.h>

#include "pixelopolis/img/create.h"

struct FlatImage* convert_royal_to_flat(const struct image* royal) {
    struct FlatImage* img = flat_image_create(royal->width, royal->height);
    if(!img) {
        return NULL;
    }


    size_t size = royal->width * royal->height;
    for(size_t i=0; i < size; i++) {
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
    }
    return img;
}
