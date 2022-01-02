#include <stdlib.h>

#include <img/destroy.h>

void destroy_image(struct image *img) {
    free(img->buffer);
    free(img);
}

void flat_image_destroy(struct FlatImage *img) {
    free(img->buffer);
    free(img);
}

void one_chan_image_destroy(struct OneChanImage* img) {
    free(img->buffer);
    free(img);
}
