#pragma once
#include <stdio.h>
#include "hash.h"

#define REGULES_SIZE 255
#define PROPS_SIZE 16
#define OBJS_SIZE 8

#define CSS_CONCAT(a, b) _CSS_CONCAT(a, b)
#define _CSS_CONCAT(a, b) a ## b
#define css_iter(obj, container) \
    int CSS_CONCAT( css_iter_i, __LINE__ ) = 0; \
    while(obj = container[CSS_CONCAT(css_iter_i, __LINE__)++])
#define OBJ_DYNAMIC (1 << 5)
#define OBJ_BINARY_OP (1 << 4)

enum ObjType {
    OBJ_NUMBER = 0,
    OBJ_STRING = 1,
    OBJ_VARIABLE = 2,
    OBJ_RULE = 3,
    OBJ_PERCENT = 4,
    OBJ_ADD = OBJ_DYNAMIC | OBJ_BINARY_OP | 0,
    OBJ_SUB = OBJ_DYNAMIC | OBJ_BINARY_OP | 1,
    OBJ_MUL = OBJ_DYNAMIC | OBJ_BINARY_OP | 2,
    OBJ_DIV = OBJ_DYNAMIC | OBJ_BINARY_OP | 3,
    OBJ_FUNC = OBJ_DYNAMIC | 4,
};

struct Obj {
    enum ObjType type;
    void *value;
};

struct PairObj {
    struct Obj *left, *right;
};

struct FuncObj {
    char* name;
    struct Obj** args;
    size_t args_size;
};

struct RuleSelector {
    char* element;
    char* klass;
    char* pseudo_klass;
    // todo - support attributes el[x="foobar"]
};
extern struct RuleSelector default_selector;

struct Rule {
    struct RuleSelector* selector;
    struct HashMap *props;
};

struct Program {
    char* name;
    struct Rule **rules;
};

struct Program* css_parse_file(char *filename);
struct Program* css_parse_file_as_stream(FILE *stream);
