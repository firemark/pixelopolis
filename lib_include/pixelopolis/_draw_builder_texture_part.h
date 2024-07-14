#pragma once
#include "pixelopolis/draw_builder_texture.h"

static inline int builder_texture_get_metric_by_direction(struct BasicTexObj* basic,
                                                          enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            return basic->width;
        case TEX_PART_HORIZONTAL:
            return basic->height;
        default:
            return 0;
    }
}

static inline int builder_texture_get_cometric_by_direction(struct BasicTexObj* basic,
                                                            enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            return basic->height;
        case TEX_PART_HORIZONTAL:
            return basic->width;
        default:
            return 0;
    }
}

static inline int builder_texture_get_pair_axis_by_direction(enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            return 0;
        case TEX_PART_HORIZONTAL:
            return 1;
        default:
            return 0;
    }
}

static inline int builder_texture_get_pair_coaxis_by_direction(enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            return 1;
        case TEX_PART_HORIZONTAL:
            return 0;
        default:
            return 0;
    }
}

static inline void builder_texture_resize_axis_by_direction(struct BasicTexObj* a, int len,
                                                            enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            a->width = a->width >= len ? a->width : len;
            break;
        case TEX_PART_HORIZONTAL:
            a->height = a->height >= len ? a->height : len;
            break;
    }
}

static inline void builder_texture_resize_axis(struct BasicTexObj* a, struct BasicTexObj* b,
                           enum TexPartDirection direction) {
#define SET_GREATER(attr) a->attr = a->attr >= b->attr ? a->attr : b->attr
    switch (direction) {
        case TEX_PART_VERTICAL:
            SET_GREATER(width);
            break;
        case TEX_PART_HORIZONTAL:
            SET_GREATER(height);
            break;
    }
#undef SET_GREATER
}

static inline void builder_texture_resize_coaxis(struct BasicTexObj* a, struct BasicTexObj* b,
                           enum TexPartDirection direction) {
#define SET_GREATER(attr) a->attr = a->attr >= b->attr ? a->attr : b->attr
    switch (direction) {
        case TEX_PART_VERTICAL:
            SET_GREATER(height);
            break;
        case TEX_PART_HORIZONTAL:
            SET_GREATER(width);
            break;
    }
#undef SET_GREATER
}

static inline void builder_texture_fill_shift(struct ShiftTexPair* pair, int shift, enum TexPartDirection direction) {
    switch (direction) {
        case TEX_PART_VERTICAL:
            pair->shift[0] = shift;
            pair->shift[1] = 0;
            break;
        case TEX_PART_HORIZONTAL:
            pair->shift[0] = 0;
            pair->shift[1] = shift;
            break;
    }
}