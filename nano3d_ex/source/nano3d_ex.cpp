#include "../nano3d_ex.h"

n3d_rasterizer_t* n3d_rasterizer_new(n3d_rasterizer_e type)
{
    // rasterizer prototypes
    extern void n3d_raster_rgb_raster(
        const n3d_rasterizer_t::state_t& state,
        const n3d_rasterizer_t::triangle_t& triangle,
        void* user);

    extern void n3d_raster_texture_raster(
        const n3d_rasterizer_t::state_t& state,
        const n3d_rasterizer_t::triangle_t& triangle,
        void* user);

    extern void n3d_raster_depth_raster(
        const n3d_rasterizer_t::state_t& state,
        const n3d_rasterizer_t::triangle_t& triangle,
        void* user);

    // return structure
    n3d_rasterizer_t rast = {
        nullptr,
        nullptr
    };

    // dispatch
    switch (type) {
    case (n3d_raster_texture):
        rast.raster_proc_ = n3d_raster_texture_raster;
        return new n3d_rasterizer_t(rast);

    case (n3d_raster_reference):
        rast.raster_proc_ = n3d_raster_rgb_raster;
        return new n3d_rasterizer_t(rast);

    case (n3d_raster_depth):
        rast.raster_proc_ = n3d_raster_depth_raster;
        return new n3d_rasterizer_t(rast);

    default:
        return nullptr;
    }
}

void n3d_rasterizer_delete(n3d_rasterizer_t* r)
{
    if (r)
        delete r;
}
