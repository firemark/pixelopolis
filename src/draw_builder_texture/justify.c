#include <stdlib.h>
#include <string.h>

#include "pixelopolis/_draw_builder_texture.h"
#include "pixelopolis/_draw_builder_texture_part.h"
#include "pixelopolis/css_func.h"

#define COSHIFT(pair, direction) \
    pair->shift[builder_texture_get_pair_coaxis_by_direction(direction)]
#define COMETRIC(basic, direction) builder_texture_get_cometric_by_direction(basic, direction)

// TODO - move to utils, remove doubled code
static inline enum Justify _get_justify(const char* name) {
#define IF_NAME(s) else if (!strcmp(name, s))
    if (!name) return JUSTIFY_START;
    IF_NAME("start") return JUSTIFY_START;
    IF_NAME("center") return JUSTIFY_CENTER;
    IF_NAME("end") return JUSTIFY_END;
    IF_NAME("random") return JUSTIFY_RANDOM;
#undef IF_NAME
    return JUSTIFY_START;
}

static inline void _do_justify(struct ShiftTexPair* pair, const enum Justify justify, int margin,
                               enum TexPartDirection direction) {
    switch (justify) {
        case JUSTIFY_START:
            break;  // do nothing
        case JUSTIFY_END:
            COSHIFT(pair, direction) += margin;
            break;
        case JUSTIFY_CENTER:
            COSHIFT(pair, direction) += margin / 2;
            break;
        case JUSTIFY_RANDOM:
            if (margin > 0) COSHIFT(pair, direction) += rand() % +margin;
            if (margin < 0) COSHIFT(pair, direction) -= rand() % -margin;
            break;
    }
}

void builder_texture_justify(struct Helper* helper, struct ShiftTexPair** pairs,
                             struct BasicTexObj* basic, enum TexPartDirection direction) {
    char* justify_selector = css_find_selector_element_prop(helper->rule, "justify");
    enum Justify justify = _get_justify(justify_selector);
    if (!pairs) return;
    struct ShiftTexPair* pair = NULL;
    size_t index = 0;
    int height = COMETRIC(basic, direction);
    while ((pair = pairs[index++])) {
        int obj_height = COMETRIC(&pair->obj->basic, direction);
        int margin = height - obj_height - COSHIFT(pair, direction);
        _do_justify(pair, justify, margin, direction);
    }
}