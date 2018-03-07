#pragma once

#include "../nano3d.h"

enum {
    e_prepare_depth = 0x01,
    e_prepare_rgb   = 0x02,
    e_prepare_uv    = 0x04,
    e_prepare_pos   = 0x08,
};

// convert a triangle from normalized device coordinates to barcentric
// coordinates required for rasterization.

bool n3d_prepare(
    n3d_rasterizer_t::triangle_t& tri,
    const n3d_vertex_t& v0,
    const n3d_vertex_t& v1,
    const n3d_vertex_t& v2,
    const uint32_t flags);
