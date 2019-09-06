#pragma once
#include "basic.h"
#include "css.h"

enum DrawObjType {
    DRAW_OBJ_VOID,
    DRAW_OBJ_CUBE,
    DRAW_OBJ_TRIANGLE,
    DRAW_OBJ_PYRAMID,
    DRAW_OBJ_SERIES,
    //DRAW_OBJ_FLOOR,
    //DRAW_OBJ_WALL,
    //DRAW_OBJ_TEXTURE,
};

enum FillDirection { VERTICAL_FILL = 0, HORIZONTAL_FILL = 2, DEPTH_FILL = 1 };

struct DrawObj {
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

struct BasicObj {
    int width, height, depth;
    int rotation;
};

struct VoidObj {
    struct BasicObj basic;
    struct DrawObj *child;
};

struct CubeObj {
    struct BasicObj basic;
    struct WallObj *south_wall, *east_wall, *roof;
};

struct TriangleObj {
    struct BasicObj basic;
    struct WallObj *wall, *roof;
};

struct PyramidObj {
    struct BasicObj basic;
    struct WallObj *east_wall;
    struct WallObj *south_wall;
};

struct SeriesObj {
    struct BasicObj basic;
    struct DrawObj *left;
    struct DrawObj **objs;
    struct DrawObj *right;
    enum FillDirection fill_direction;
    int padding;
};

void builder_init(void);
void builder_stop(void);
struct DrawObj* builder_make(struct Program* program, struct Rule* world);
