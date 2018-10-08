#pragma once

struct rgb {unsigned char r, g, b;};
struct hsv {unsigned char h, s, v;};

struct image {
    struct rgb* buffer;
    int width;
    int height;
};
