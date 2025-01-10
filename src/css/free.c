#include <stdlib.h>

#include "pixelopolis/css/func.h"
#include "pixelopolis/hash.h"

void css_free_program(struct Program* program) {
    memory_free(program->syntax_memory);
    memory_free(program->product_memory);
    free(program->name);
    free(program);
}
