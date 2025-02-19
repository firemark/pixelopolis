#pragma once
#include <stdio.h>

#include "hash.h"
#include "memory.h"

#define REGULES_SIZE 255
#define KLASSES_SIZE 16
#define RULE_ATTRS_SIZE 64
#define OBJS_SIZE 16
#define RULE_SELECTORS_SIZE 64

#define CSS_CONCAT(a, b) _CSS_CONCAT(a, b)
#define _CSS_CONCAT(a, b) a##b
#define css_iter(obj, container)              \
    int CSS_CONCAT(css_iter_i, __LINE__) = 0; \
    while ((obj = container[CSS_CONCAT(css_iter_i, __LINE__)++]))
#define OBJ_DYNAMIC (1 << 5)
#define OBJ_BINARY_OP (1 << 4)

enum ObjType {
    OBJ_NUMBER = 0,
    OBJ_STRING = 1,
    OBJ_VARIABLE = 2,
    OBJ_RULE = 3,
    OBJ_PERCENT = 4,
    OBJ_COLOR = 5,
    OBJ_ADD = OBJ_DYNAMIC | OBJ_BINARY_OP | 0,
    OBJ_SUB = OBJ_DYNAMIC | OBJ_BINARY_OP | 1,
    OBJ_MUL = OBJ_DYNAMIC | OBJ_BINARY_OP | 2,
    OBJ_DIV = OBJ_DYNAMIC | OBJ_BINARY_OP | 3,
    OBJ_FUNC = OBJ_DYNAMIC | 4,
};

struct Obj {
    enum ObjType type;
    void* value;
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
    char** klasses;
    char* pseudo_klass;
    struct RuleSelector* parent;
    struct RuleSelector* greedy_parent;
    // todo - support attributes el[x="foobar"]
};
extern struct RuleSelector default_selector;

struct Rule {
    struct RuleSelector* selector;
    struct HashMap* props;
};

struct RuleWithParent {
    struct Rule* rule;
    struct RuleWithParent* parent;
};

typedef void (*ProgramLogFunc)(char severity, char* msg, char* filename, int line, int row);

struct Program {
    char* name;
    struct Rule** rules;
    struct Memory* syntax_memory;
    struct Memory* product_memory;
    ProgramLogFunc log;
};

struct Program* css_parse_file(char* filename, ProgramLogFunc log);
struct Program* css_parse_file_as_string(char* code, ProgramLogFunc log);
struct Program* css_parse_file_as_stream(FILE* stream, ProgramLogFunc log);
