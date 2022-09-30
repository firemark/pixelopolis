#include <stdlib.h>
#include "pixelopolis/_draw_builder.h"

struct BoardChild* builder_build_board_child(struct DrawObj* obj, int x, int y) {
    struct BoardChild* child = malloc(sizeof(struct BoardChild));
    child->obj = obj;
    child->x = x;
    child->y = y;
    return child;
}
