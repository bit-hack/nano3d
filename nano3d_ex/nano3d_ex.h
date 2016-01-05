#pragma once

#include "source/n3d_forward.h"
#include "nano3d.h"

enum n3d_rasterizer_e {

    n3d_raster_reference,
    n3d_raster_depth,
};

n3d_rasterizer_t * n3d_rasterizer_new(n3d_rasterizer_e);
void n3d_rasterizer_delete(n3d_rasterizer_t *);
