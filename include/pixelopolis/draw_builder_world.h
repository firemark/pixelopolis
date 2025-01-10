#pragma once
#include "pixelopolis/basic.h"
#include "pixelopolis/css.h"
#include "pixelopolis/draw_builder_common.h"

struct ShiftDrawPair {
    int shift;
    struct DrawObj *obj;
};

struct WallObj {
    struct TexObj *tex, *points_tex;
    int floors_size;
    int padding;
    int points_density;
    int width;
    int height;
};

struct VoidObj {
    struct DrawObj *child;
};

struct CubeObj {
    struct WallObj *south_wall, *east_wall, *roof;
};

struct CylinderObj {
    struct WallObj *roof, **walls;
    int has_many_walls;
    int sides;
};

struct ConeObj {
    struct WallObj *roof;
    int sides;
};

struct DomeObj {
    struct WallObj *roof;
    int vertical_sides;
    int horizontal_sides;
};

struct TriangleObj {
    struct WallObj *wall, *roof;
};

struct PyramidObj {
    struct WallObj *east_wall;
    struct WallObj *south_wall;
};

struct SeriesObj {
    struct ShiftDrawPair **pairs;
    enum FillDirection fill_direction;
};

struct BoardChild {
    struct DrawObj *obj;
    int x, y;
};

struct BoardObj {
    struct BoardChild **children;
};

void builder_init(void);
void builder_stop(void);
struct DrawObj *builder_make(struct Program *program, struct Rule *world);
