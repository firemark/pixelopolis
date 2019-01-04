#pragma once

struct rgb {unsigned char r, g, b;};
struct hsv {unsigned char h, s, v;};
enum direction {DIRECTION_UP, DIRECTION_EAST, DIRECTION_SOUTH};

struct image {
    struct rgb* buffer;
    int width;
    int height;
};
