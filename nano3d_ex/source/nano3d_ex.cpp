#include "../nano3d_ex.h"

#define RASTER_PROTO(NAME)                            \
    void NAME(                                        \
        const n3d_rasterizer_t::state_t& state,       \
        const n3d_rasterizer_t::triangle_t& triangle, \
        void* user);

// rasterizer prototypes
RASTER_PROTO(n3d_raster_rgb_raster)
RASTER_PROTO(n3d_raster_texture_raster)
RASTER_PROTO(n3d_raster_depth_raster)
RASTER_PROTO(n3d_raster_depth_raster_sse)

n3d_rasterizer_t* n3d_rasterizer_new(n3d_rasterizer_e type)
{
    // return structure
    n3d_rasterizer_t rast = {nullptr, nullptr};

    // dispatch
    switch (type) {
    case n3d_raster_texture:
        rast.raster_proc_ = n3d_raster_texture_raster;
        return new n3d_rasterizer_t(rast);
    case n3d_raster_rgb:
        rast.raster_proc_ = n3d_raster_rgb_raster;
        return new n3d_rasterizer_t(rast);
    case n3d_raster_depth:
        rast.raster_proc_ = n3d_raster_depth_raster;
        return new n3d_rasterizer_t(rast);
    case n3d_raster_depth_sse:
        rast.raster_proc_ = n3d_raster_depth_raster_sse;
        return new n3d_rasterizer_t(rast);
    default:
        return nullptr;
    }
}

void n3d_rasterizer_delete(n3d_rasterizer_t* r)
{
    if (r) {
        delete r;
    }
}
