#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pixelopolis/basic.h"
#include "pixelopolis/img.h"
#include "pixelopolis/img/normal_map.h"

int main(int argc, char** argv) {
    char* in_filename = argc > 1 ? argv[1] : "in.png";
    char* out_filename = argc > 2 ? argv[2] : "out.png";

    struct OneChanImage* bump_map = one_chan_image_read_png_file(in_filename);
    if (!bump_map) {
        fprintf(stderr, "Problem with reading file %s\n", out_filename);
        return -1;
    }

    struct FloatImage* normal_map = transform_bump_to_normal_map(bump_map);
    if (!normal_map) {
        fprintf(stderr, "Problem with transforming normal map.\n");
        return -1;
    }

    struct FlatImage* rgb_normal_map = tranform_normal_map_to_rgb_map(normal_map);
    if (!rgb_normal_map) {
        fprintf(stderr, "Problem with transforming normal map.\n");
        return -1;
    }

    FILE* fp = fopen(out_filename, "wb");
    if (!fp) {
        fprintf(stderr, "Problem with creating file %s\n", out_filename);
        return -1;
    }

    write_png_file_from_flat_image(fp, rgb_normal_map);
    flat_image_destroy(rgb_normal_map);
    float_image_destroy(normal_map);
    one_chan_image_destroy(bump_map);

    return 0;
}
