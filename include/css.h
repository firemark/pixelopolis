#pragma once

#define REGULES_SIZE 255
#define PROPS_SIZE 16
#define OBJS_SIZE 8

enum ObjType {
    OBJ_NUMBER,
    OBJ_STRING,
    OBJ_REGULE_NAME,
};

struct Obj {
    enum ObjType type;
    void *value;
};

struct Prop {
    char* name;
    struct Obj **objs;
};

struct Regule {
    char* name;
    struct Prop **props;
};

struct Program {
    char* name;
    struct Regule **regules;
};

struct Program* css_parse_file(char *filename);
