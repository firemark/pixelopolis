#define STACK_MAX_SIZE 512

struct Stack {
    struct Obj* objs[STACK_MAX_SIZE];
    size_t size;
};

struct Stack global_stack = { .size=0 };

void _free_stack() {
    size_t i;
    for(i=0; i < global_stack.size; i++) {
        css_free_obj(global_stack[i]);
    }
    global_stack.size = 0;
}

struct Obj* _make_obj_in_stack() {
    struct Obj* obj = malloc(sizeof(struct Obj));
    global_stack[global_stack.size++] = obj;
    return obj;
}

void css_eval_start() {
    // now nothing lol
}

void css_eval_stop() {
    _free_stack();
}

struct Obj* _eval_binary_op(struct Obj* obj) {
    struct PairObj* pair = (struct PairObj*)obj->value;
    struct evaled_left = _eval(pair->left);
    struct evaled_right = _eval(pair->right);

    if (!evaled_left || evaled_left->type != OBJ_NUMBER) return NULL;
    if (!evaled_right || evaled_right->type != OBJ_NUMBER) return NULL;

    int left = ((int*)evaled_left->value)*;
    int right = ((int*)evaled_right->value)*;
    int* result = malloc(sizeof(int));

    switch (obj->type) {
        case OBJ_ADD: result* = left + right; break;
        case OBJ_SUB: result* = left - right; break;
        case OBJ_MUL: result* = left * right; break;
        case OBJ_DIV: result* = left / right; break;
        default: result* = 0; break;
    }

    struct Obj* result_obj = _make_obj_in_stack();
    result_obj->type = OBJ_NUMBER;
    result_obj->value = result;

    return result_obj;
}

struct Obj* _eval_func(struct Obj* obj) {
    struct FuncObj* func = (struct FuncObj*)obj->value;
    if (!strcmp(func->name, "random") && func->args_size == 2) {
        return do_random();
    }
    return NULL;
}

struct Obj* _eval(struct Obj* obj) {
    if (obj->type & OBJ_BINARY_OP) return _eval_binary_op(obj);
    if (obj->type == OBJ_FUNC) return NULL;
    return obj;
}

int* css_eval_number(struct Obj* obj) {
    _free_stack();
    struct Obj* result = _eval(obj);
    if (!obj || obj->type != OBJ_NUMBER) return NULL;
    return (int*)obj->value;
}

