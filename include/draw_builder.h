#pragma once
#include "basic.h"
#include "css.h"

enum DrawObjType {
    DRAW_OBJ_VOID,
    DRAW_OBJ_CUBE,
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

struct TexObj {
    struct FlatImage *texture;
};

struct FloorObj {
    struct TexObj *tex;
    struct TexObj **objs;
    struct TexObj *left;
    struct TexObj *right;
    int objs_size;
    int height;
    int padding;
};

struct WallObj {
    struct TexObj *tex;
    struct FloorObj **floors;
    struct FloorObj *bottom;
    struct FloorObj *top;
    int floors_size;
    int padding;
};

struct VoidObj {
    struct DrawObj *child;
};

struct CubeObj {
    struct WallObj *south_wall, *east_wall, *roof;
};

struct TriangleObj {
    struct WallObj *wall, *roof;
};

struct PyramidObj {
    struct WallObj *east_wall;
    struct WallObj *south_wall;
};

struct SeriesObj {
    struct DrawObj *left;
    struct DrawObj **objs;
    struct DrawObj *right;
    enum FillDirection fill_direction;
    int padding;
};

void builder_init(void);
void builder_stop(void);
struct DrawObj* builder_make(struct Program* program, struct Rule* world);
