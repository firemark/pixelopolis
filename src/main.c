#include <basic.h>
#include <stdlib.h>

#include <img.h>


int main(int argc, char **argv) {
    struct image img = {
        .width=800,
        .height=800,
        .buffer=malloc(800 * 800 * sizeof(struct rgb))};
    int x, y;
    for (x=0; x < img.width; x++)
        for (y=0; y < img.height; y++) {
            struct rgb temp = {.r=x & 0xff, .g=y & 0xff, .b=(x / 0xff * 64) & 0xff};
            img.buffer[y * img.width + x] = temp;
        }
    write_png_file("out.png", &img);
    return 0;
}
