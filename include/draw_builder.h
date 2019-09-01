#pragma once
#include "basic.h"
#include "css.h"

enum DrawObjType {
    DRAW_OBJ_CUBE,
    DRAW_OBJ_TRIANGLE,
    DRAW_OBJ_PYRAMID,
    DRAW_OBJ_SERIES,
    //DRAW_OBJ_FLOOR,
    //DRAW_OBJ_WALL,
    //DRAW_OBJ_TEXTURE,
};

enum Series { VERTICAL_SERIES=0, HORIZONTAL_SERIES=1, DEPTH_SERIES=2 };

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
    struct DrawObj **objs;
    enum Series series;
    int padding;
};

void builder_init(void);
void builder_stop(void);
struct DrawObj* builder_make(struct Program* program, struct Rule* world);
