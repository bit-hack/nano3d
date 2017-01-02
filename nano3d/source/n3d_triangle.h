#pragma once

#include "../nano3d.h"

enum {
    e_prepare_depth = 1,
    e_prepare_rgb   = 2,
    e_prepare_uv    = 4,
    e_prepare_pos   = 8,
};

bool n3d_prepare(
    n3d_rasterizer_t::triangle_t& tri,
    const n3d_vertex_t& v0,
    const n3d_vertex_t& v1,
    const n3d_vertex_t& v2,
    int flags);
