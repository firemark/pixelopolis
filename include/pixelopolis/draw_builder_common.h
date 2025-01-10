#pragma once
enum FillDirection { X_AXIS_FILL = 0, HORIZONTAL_FILL = 2, Y_AXIS_FILL = 1 };
enum Justify { JUSTIFY_START, JUSTIFY_CENTER, JUSTIFY_END, JUSTIFY_RANDOM };

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

enum TexObjType {
    TEX_OBJ_VOID,
    TEX_OBJ_FILL,
    TEX_OBJ_DEFAULT,
    TEX_OBJ_PART,
};

struct BasicObj {
    int width, height, depth;
    int rotate;
    enum Justify v_justify;
    enum Justify d_justify;
    double sin_th, cos_th;
};

struct DrawObj {
    struct BasicObj basic;
    enum DrawObjType type;
    void* obj;
    struct DrawObj* parent;
};

struct BasicTexObj {
    int width, height;
};

struct TexObj {
    struct BasicTexObj basic;
    enum TexObjType type;
    void* obj;
    struct TexObj* parent;
};