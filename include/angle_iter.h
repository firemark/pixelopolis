struct AngleIter {
    int x, y;
    int nx, ny;
    int w, h;
    int i, sides;
    double theta_iter;
    double start_theta;
    double theta;
};

void angle_iter_start(struct AngleIter *iter, int w, int h, int sides);
int angle_iter_iterate(struct AngleIter *iter);
int angle_iter_get_length(struct AngleIter *iter);
