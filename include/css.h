#pragma once

#define REGULES_SIZE 255
#define PROPS_SIZE 16
#define OBJS_SIZE 8

#define CSS_CONCAT(a, b) _CSS_CONCAT(a, b)
#define _CSS_CONCAT(a, b) a ## b
#define css_iter(obj, container) \
    int CSS_CONCAT( css_iter_i, __LINE__ ) = 0; \
    while(obj = container[CSS_CONCAT(css_iter_i, __LINE__)++])

enum ObjType {
    OBJ_NUMBER,
    OBJ_STRING,
    OBJ_RULE,
};

struct Obj {
    enum ObjType type;
    void *value;
};

struct Prop {
    char* name;
    struct Obj **objs;
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
    struct Prop **props;
};


struct Program {
    char* name;
    struct Rule **rules;
};

struct Program* css_parse_file(char *filename);
