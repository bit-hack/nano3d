#pragma once

#include "../nano3d.h"

bool n3d_prepare(
    n3d_rasterizer_t::triangle_t & tri,
    const n3d_vertex_t & v0,
    const n3d_vertex_t & v1,
    const n3d_vertex_t & v2);
