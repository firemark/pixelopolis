#include <math.h>

#include "angle_iter.h"

void _angle_iter_update_ncords(struct AngleIter *iter) {
    iter->nx = cos(iter->theta) * iter->w;
    iter->ny = sin(iter->theta) * iter->h;
}

void angle_iter_start_with_range(struct AngleIter *iter, int w, int h, int sides, int start_sides, int end_sides) {
    iter->theta_iter = 2.0 * M_PI / (double)sides;
    iter->end_sides = end_sides;
    iter->i = start_sides - 1;
    iter->start_theta = iter->theta_iter * start_sides;
    iter->theta = iter->start_theta;
    iter->w = w / 2;
    iter->h = h / 2;
    _angle_iter_update_ncords(iter);
}

void angle_iter_start(struct AngleIter *iter, int w, int h, int sides) {
    angle_iter_start_with_range(iter, w, h, sides, 0, sides);
}

int angle_iter_iterate(struct AngleIter *iter) {
    if (iter->i > iter->end_sides) {
        return 0;
    }

    iter->x = iter->nx;
    iter->y = iter->ny;
    iter->i++;

    if (iter->i == iter->sides) {
        iter->theta = iter->start_theta;
    } else {
        iter->theta += iter->theta_iter;
    }

    _angle_iter_update_ncords(iter);

    return 1;
}

int angle_iter_get_length(struct AngleIter *iter) {
    int dx = iter->x - iter->nx;
    int dy = iter->y - iter->ny;
    return sqrt(dx * dx + dy * dy);
}
