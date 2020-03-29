#pragma once

struct AngleIter {
    int x, y;
    int nx, ny;
    int w, h;
    int i, sides, end_sides;
    double theta_iter;
    double start_theta;
    double theta;
};

void angle_iter_start(struct AngleIter *iter, int w, int h, int sides);
void angle_iter_start_with_range(struct AngleIter *iter, int w, int h, int sides, int start_sides, int end_sides);
int angle_iter_iterate(struct AngleIter *iter);
int angle_iter_get_length(struct AngleIter *iter);
