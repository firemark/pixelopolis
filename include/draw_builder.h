#pragma once
#include "basic.h"
#include "css.h"

enum DrawObjType {
    DRAW_OBJ_VOID,
    DRAW_OBJ_CUBE,
    DRAW_OBJ_CYLINDER,
    DRAW_OBJ_TRIANGLE,
    DRAW_OBJ_PYRAMID,
    DRAW_OBJ_SERIES,
};

enum FillDirection { VERTICAL_FILL = 0, HORIZONTAL_FILL = 2, DEPTH_FILL = 1 };

struct BasicObj {
    int width, height, depth;
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
    struct TexObj *tex;
    struct FloorObj **floors;
    struct FloorObj *bottom;
    struct FloorObj *top;
    int floors_size;
    int padding;
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

void builder_init(void);
void builder_stop(void);
struct DrawObj* builder_make(struct Program* program, struct Rule* world);
