#include <basic.h>
#include <stdlib.h>

#include <img.h>


int main(int argc, char **argv) {
    int w = 800;
    int h = 600;
    struct rgb* buffer = malloc(w * h * sizeof(struct rgb));
    int x, y;
    for (x=0; x < w; x++)
        for (y=0; y < h; y++) {
            struct rgb temp = {.r=x & 0xff, .g=y & 0xff, .b=(x / 0xff * 64) & 0xff};
            buffer[y * w + x] = temp;
        }
    write_png_file("out.png", w, h, buffer);
    return 0;
}
