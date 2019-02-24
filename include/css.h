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

struct Rule {
    char* name;
    struct Prop **props;
};

struct Program {
    char* name;
    struct Rule **rules;
};

struct Program* css_parse_file(char *filename);
void css_debug_program(FILE* fp, struct Program* program);
void css_debug_rule(FILE* fp, struct Rule* rule);
void css_debug_prop(FILE* fp, struct Prop* prop);
