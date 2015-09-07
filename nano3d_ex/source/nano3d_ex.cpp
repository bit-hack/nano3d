#include "../nano3d_ex.h"

extern
void n3d_raster_reference_raster(
    const n3d_rasterizer_t::state_t & state,
    const n3d_rasterizer_t::triangle_t & triangle,
    void * user);

n3d_rasterizer_t * n3d_rasterizer_new(n3d_rasterizer_e type) {

    n3d_rasterizer_t rast = {
        nullptr,
        nullptr
    };

    switch (type) {
    case (n3d_raster_reference) :
        rast.raster_proc_ = n3d_raster_reference_raster;
        return new n3d_rasterizer_t(rast);

    default:
        return nullptr;
    }
}

void n3d_rasterizer_delete(n3d_rasterizer_t *r) {
    if (r) delete r;
}
