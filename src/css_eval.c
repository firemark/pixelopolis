#include "pixelopolis/css_eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pixelopolis/css_func.h"

static struct Obj* _eval(struct Obj* obj);

static struct Memory *_memory;

static void _free_stack() {
    memory_free(_memory);
    _memory = memory_create(1024 * 1024);
}

void css_eval_start() {
    _memory = memory_create(1024 * 1024);
    int grain = time(NULL);
    srand(grain);
}

void css_eval_stop() { memory_free(_memory); }

static int _is_number(struct Obj* obj) { return obj && obj->type == OBJ_NUMBER; }

static int _is_percent(struct Obj* obj) { return obj && obj->type == OBJ_PERCENT; }

static int _is_string(struct Obj* obj) { return obj && obj->type == OBJ_STRING; }

static int _is_rule(struct Obj* obj) { return obj && obj->type == OBJ_RULE; }

static struct Obj* _eval_binary_op(struct Obj* obj) {
    struct PairObj* pair = (struct PairObj*)obj->value;
    struct Obj* evaled_left = _eval(pair->left);
    struct Obj* evaled_right = _eval(pair->right);

    if (!_is_number(evaled_left)) return NULL;
    if (!_is_number(evaled_right)) return NULL;

    int left = *(int*)evaled_left->value;
    int right = *(int*)evaled_right->value;
    int* result = MEMORY_ALLOCATE(_memory, int);

    switch (obj->type) {
        case OBJ_ADD:
            *result = left + right;
            break;
        case OBJ_SUB:
            *result = left - right;
            break;
        case OBJ_MUL:
            *result = left * right;
            break;
        case OBJ_DIV:
            *result = left / right;
            break;
        default:
            *result = 0;
            break;
    }

    struct Obj* result_obj = MEMORY_ALLOCATE(_memory, struct Obj);
    result_obj->type = OBJ_NUMBER;
    result_obj->value = result;

    return result_obj;
}

static struct Obj* _do_random(struct FuncObj* func) {
    if (func->args_size != 2) return NULL;
    struct Obj* evaled_start = _eval(func->args[0]);
    struct Obj* evaled_end = _eval(func->args[1]);

    if (!_is_number(evaled_start)) return NULL;
    if (!_is_number(evaled_end)) return NULL;

    int start = *(int*)evaled_start->value;
    int end = *(int*)evaled_end->value;
    if (start > end) {
        int t = start;
        start = end;
        end = t;
    }
    int* result = MEMORY_ALLOCATE(_memory, int);
    *result = start == end ? start : start + (rand() % (end - start));

    struct Obj* result_obj = MEMORY_ALLOCATE(_memory, struct Obj);
    result_obj->type = OBJ_NUMBER;
    result_obj->value = result;

    return result_obj;
}

static struct Obj* _do_choice(struct FuncObj* func) {
    if (func->args_size == 0) return NULL;
    size_t index = rand() % func->args_size;
    struct Obj* arg = func->args[index];
    return _eval(arg);
}

static struct Obj* _do_rgb(struct FuncObj* func) {
    if (func->args_size != 3) return NULL;
    struct Obj* r_obj = _eval(func->args[0]);
    struct Obj* g_obj = _eval(func->args[1]);
    struct Obj* b_obj = _eval(func->args[2]);

    if (!_is_number(r_obj)) return NULL;
    if (!_is_number(g_obj)) return NULL;
    if (!_is_number(b_obj)) return NULL;

    struct rgb* color = MEMORY_ALLOCATE(_memory, struct rgb);
    color->r = *(unsigned char*)r_obj->value;
    color->g = *(unsigned char*)g_obj->value;
    color->b = *(unsigned char*)b_obj->value;

    struct Obj* result_obj = MEMORY_ALLOCATE(_memory, struct Obj);
    result_obj->type = OBJ_COLOR;
    result_obj->value = color;

    return result_obj;
}

static struct Obj* _do_to_percent(struct FuncObj* func) {
    if (func->args_size == 0) return NULL;
    struct Obj* arg = func->args[0];
    struct Obj* result_obj = _eval(arg);
    if (_is_percent(result_obj)) return result_obj;
    if (!_is_number(result_obj)) return NULL;

    result_obj->type = OBJ_PERCENT;
    return result_obj;
}

static struct Obj* _eval_func(struct Obj* obj) {
    struct FuncObj* func = (struct FuncObj*)obj->value;
#define CMP(value) !strcmp(func->name, value)
    if (CMP("random")) return _do_random(func);
    if (CMP("choice")) return _do_choice(func);
    if (CMP("to-percent")) return _do_to_percent(func);
    if (CMP("rgb")) return _do_rgb(func);
    return NULL;
#undef CMP
}

static struct Obj* _eval(struct Obj* obj) {
    if (obj->type & OBJ_BINARY_OP) return _eval_binary_op(obj);
    if (obj->type == OBJ_FUNC) return _eval_func(obj);
    return obj;
}

struct Obj* css_eval(struct Obj* obj) {
    if (!obj) return NULL;
    _free_stack();
    return _eval(obj);
}

// TODO - remove css_eval_XXX functions - are useless
int* css_eval_number(struct Obj* obj) {
    struct Obj* result = css_eval(obj);
    if (!_is_number(result)) return NULL;
    return (int*)result->value;
}

int* css_eval_percent(struct Obj* obj) {
    struct Obj* result = css_eval(obj);
    if (!_is_percent(result)) return NULL;
    return (int*)result->value;
}

char* css_eval_string(struct Obj* obj) {
    struct Obj* result = css_eval(obj);
    if (!_is_string(result)) return NULL;
    return (char*)result->value;
}

struct RuleSelector* css_eval_rule(struct Obj* obj) {
    struct Obj* result = css_eval(obj);
    if (!_is_rule(result)) return NULL;
    return (struct RuleSelector*)result->value;
}
