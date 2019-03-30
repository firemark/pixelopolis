#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "css_func.h"
#include "css_eval.h"

#define STACK_MAX_SIZE 512

struct Stack {
    struct Obj* objs[STACK_MAX_SIZE];
    size_t size;
};

struct Stack global_stack = { .size=0 };
struct Obj* _eval(struct Obj* obj);

void _free_stack() {
    size_t i;
    for(i=0; i < global_stack.size; i++) {
        css_free_obj(global_stack.objs[i]);
    }
    global_stack.size = 0;
}

struct Obj* _make_obj_in_stack() {
    struct Obj* obj = malloc(sizeof(struct Obj));
    size_t index = global_stack.size++;
    global_stack.objs[index] = obj;
    return obj;
}

void css_eval_start() {
    int grain = time(NULL);
    srand(grain);
}

void css_eval_stop() {
    _free_stack();
}

int _is_number(struct Obj* obj) {
    return obj && obj->type == OBJ_NUMBER;
}

struct Obj* _eval_binary_op(struct Obj* obj) {
    struct PairObj* pair = (struct PairObj*)obj->value;
    struct Obj* evaled_left = _eval(pair->left);
    struct Obj* evaled_right = _eval(pair->right);

    if (!_is_number(evaled_left)) return NULL;
    if (!_is_number(evaled_right)) return NULL;

    int left = *(int*)evaled_left->value;
    int right = *(int*)evaled_right->value;
    int* result = malloc(sizeof(int));

    switch (obj->type) {
        case OBJ_ADD: *result = left + right; break;
        case OBJ_SUB: *result = left - right; break;
        case OBJ_MUL: *result = left * right; break;
        case OBJ_DIV: *result = left / right; break;
        default: *result = 0; break;
    }
    

    struct Obj* result_obj = _make_obj_in_stack();
    result_obj->type = OBJ_NUMBER;
    result_obj->value = result;

    return result_obj;
}

struct Obj* _do_random(struct FuncObj* func) {
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
    int* result = malloc(sizeof(int));
    *result = start == end ? start : start + (rand() % (end - start));

    struct Obj* result_obj = _make_obj_in_stack();
    result_obj->type = OBJ_NUMBER;
    result_obj->value = result;

    return result_obj;
}

struct Obj* _do_choice(struct FuncObj* func) {
    if (func->args_size == 0) return NULL;
    int index = rand() % func->args_size;
    struct Obj* arg = func->args[index];
    return _eval(arg);
}

struct Obj* _eval_func(struct Obj* obj) {
    struct FuncObj* func = (struct FuncObj*)obj->value;
    if (!strcmp(func->name, "random")) {
        return _do_random(func);
    }
    if (!strcmp(func->name, "choice")) {
        return _do_choice(func);
    }
    return NULL;
}

struct Obj* _eval(struct Obj* obj) {
    if (obj->type & OBJ_BINARY_OP) return _eval_binary_op(obj);
    if (obj->type == OBJ_FUNC) return _eval_func(obj);
    return obj;
}

int* css_eval_number(struct Obj* obj) {
    _free_stack();
    struct Obj* result = _eval(obj);
    if (!_is_number(result)) return NULL;
    return (int*)result->value;
}

