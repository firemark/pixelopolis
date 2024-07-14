#include "pixelopolis/img/destroy.h"

#include <stdlib.h>

#define _DESTROY(img)      \
    {                      \
        free(img->buffer); \
        free(img);         \
    }

void destroy_image(struct image *img) _DESTROY(img);
void flat_image_destroy(struct FlatImage *img) _DESTROY(img);
void one_chan_image_destroy(struct OneChanImage *img) _DESTROY(img);
void float_image_destroy(struct FloatImage *img) _DESTROY(img);
