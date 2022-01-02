#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "basic.h"
#include "img.h"
#include "normal_map.h"

int main(int argc, char** argv) {
    char *in_filename = argc > 1 ? argv[1] : "in.png";
    char *out_filename = argc > 2 ? argv[2] : "out.png";

    struct OneChanImage* bump_map = one_chan_image_read_png_file(in_filename);
    if (!bump_map) {
        fprintf(stderr, "Problem with reading file %s\n", out_filename);
        return -1;
    }

    struct FlatImage* normal_map = transform_bump_to_normal_map(bump_map);
    if (!normal_map) {
        fprintf(stderr, "Problem with transforming normal map.\n");
        return -1;
    }

    FILE* fp = fopen(out_filename, "wb");
    if (!fp) {
        fprintf(stderr, "Problem with creating file %s\n", out_filename);
        return -1;
    }

    write_png_file_from_flat_image(fp, normal_map);
    flat_image_destroy(normal_map);
    one_chan_image_destroy(bump_map);

    return 0;
}
