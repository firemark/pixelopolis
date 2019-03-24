#pragma once
#include "basic.h"
#include "css.h"

enum DrawObjType {
    DRAW_OBJ_CUBE,
    DRAW_OBJ_TRIANGLE,
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
    struct image *texture;
};

struct FloorObj {
    struct TexObj *tex;
    int height;
    int padding;
};

struct WallObj {
    struct TexObj *tex;
    struct FloorObj *bottom, *middle, *top;
};

struct BasicObj {
    int width, height, depth;
    int rotation;
};

struct CubeObj {
    struct BasicObj basic;
    struct WallObj *wall, *roof;
};

struct TriangleObj {
    struct BasicObj basic;
    struct WallObj *wall, *roof;
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
