#pragma once
#include "basic.h"
#include "css.h"

enum DrawObjType {
    DRAW_OBJ_VOID,
    DRAW_OBJ_CUBE,
    DRAW_OBJ_CONE,
    DRAW_OBJ_DOME,
    DRAW_OBJ_CYLINDER,
    DRAW_OBJ_TRIANGLE,
    DRAW_OBJ_PYRAMID,
    DRAW_OBJ_SERIES,
    DRAW_OBJ_BOARD,
};

enum FillDirection { VERTICAL_FILL = 0, HORIZONTAL_FILL = 2, DEPTH_FILL = 1 };
enum Justify { JUSTIFY_START, JUSTIFY_CENTER, JUSTIFY_END, JUSTIFY_RANDOM };

struct BasicObj {
    int width, height, depth;
    int rotate;
    enum Justify v_justify, d_justify;
    double sin_th, cos_th;
};

struct DrawObj {
    struct BasicObj basic;
    enum DrawObjType type;
    void* obj;
    struct DrawObj *parent;
};

struct ShiftTexPair {
    int shift;
    struct TexObj* obj;
};

struct ShiftDrawPair {
    int shift;
    struct DrawObj* obj;
};

struct TexObj {
    struct FlatImage *texture;
    struct rgb* color;
};

struct FloorObj {
    struct TexObj *tex;
    struct ShiftTexPair **objs;
    int objs_size;
    int height;
};

struct WallObj {
    struct TexObj *tex, *points_tex;
    struct FloorObj **floors;
    struct FloorObj *bottom;
    struct FloorObj *top;
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
struct DrawObj* builder_make(struct Program* program, struct Rule* world);
