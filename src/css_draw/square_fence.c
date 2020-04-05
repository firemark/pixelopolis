#include "_css_draw.h"
#include "css_draw.h"

static inline void _draw_edge_and_corner(
        int corner_vox[3],
        int edge_vox[3],
        const struct SquareFenceObj *obj, 
        const struct DrawInnerInfo *inner_info,
        const size_t index) {
    struct DrawObj *corner = obj->corners[index];
    struct DrawObj *edge = obj->edges[index];
    const struct BasicObj *edge_basic = &edge->basic;

    const int edge_size = obj->edge_size;

    struct DrawInfo corner_draw_info = {
        .img=inner_info->img,
        .vox=corner_vox,
    };
    draw_component(corner, &corner_draw_info, NULL);

    edge_vox[0] += _x_rotate(edge_size, 0, edge_basic);
    edge_vox[1] += _y_rotate(edge_size, 0, edge_basic);

    struct DrawInfo edge_draw_info = {
        .img=inner_info->img,
        .vox=edge_vox,
    };
    draw_component(edge, &edge_draw_info, NULL);

    if (index < 3) {
        const int len = edge_basic->width + edge_size;
        const int dx = _x_rotate(len, 0, edge_basic);
        const int dy = _y_rotate(len, 0, edge_basic);

        corner_vox[0] += dx;
        corner_vox[1] += dy;
        edge_vox[0] += dx;
        edge_vox[1] += dy;
    }
}

static inline void _draw_edges_and_corners(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    const struct SquareFenceObj *obj = draw_obj->obj;
    int corner_vox[3] = COPY_VOX(inner_info->vox);
    int edge_vox[3] = COPY_VOX(inner_info->vox);

    size_t i;
    for (i=0; i < 4; i++) {
        _draw_edge_and_corner(corner_vox, edge_vox, obj, inner_info, i);
    }
}

static inline void _draw_body(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    const struct SquareFenceObj *obj = draw_obj->obj;
    const struct BasicObj *basic = &draw_obj->basic;
    struct DrawObj *body = obj->body;

    const int edge_size = obj->edge_size;

    int vox[3] = COPY_VOX(inner_info->vox);
    vox[0] += _x_rotate(edge_size, edge_size, basic);
    vox[1] += _y_rotate(edge_size, edge_size, basic);

    struct DrawInfo draw_info = {
        .img=inner_info->img,
        .vox=vox,
    };
    draw_component(body, &draw_info, NULL);
}


void css_draw_square_fence(struct DrawObj *draw_obj, struct DrawInnerInfo *inner_info) {
    _draw_edges_and_corners(draw_obj, inner_info);
    _draw_body(draw_obj, inner_info);

    int *out_vox = inner_info->out_vox;
    if (out_vox) {
        out_vox[0] = draw_obj->basic.width;
        out_vox[1] = draw_obj->basic.depth;
        out_vox[2] = draw_obj->basic.height;
    }
}
