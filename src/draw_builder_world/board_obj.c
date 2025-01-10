#include <stdlib.h>

#include "pixelopolis/_draw_builder_world.h"

struct BoardChild* builder_build_board_child(struct Helper* helper, struct DrawObj* obj, int x,
                                             int y) {
    struct BoardChild* child = HELPER_ALLOCATE(helper, struct BoardChild);
    child->obj = obj;
    child->x = x;
    child->y = y;
    return child;
}
