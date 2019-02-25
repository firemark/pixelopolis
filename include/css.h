#pragma once

#define REGULES_SIZE 255
#define PROPS_SIZE 16
#define OBJS_SIZE 8

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

struct Rule {
    struct RuleSelector* selector;
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
void css_debug_obj(FILE* fp, struct Obj* obj);
void css_debug_rule_selector(FILE* fp, struct RuleSelector* selector);
