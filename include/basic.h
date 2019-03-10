#pragma once

#define COPY_VOX(vox) {vox[0], vox[1], vox[2]}
#define ZERO_VOX {0, 0, 0}

struct rgb {unsigned char r, g, b; int zindex;};
struct hsv {unsigned char h, s, v;};
enum direction {DIRECTION_UP, DIRECTION_EAST, DIRECTION_SOUTH};

struct image {
    struct rgb* buffer;
    int width;
    int height;
};
