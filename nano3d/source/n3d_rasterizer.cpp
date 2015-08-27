#include "nano3d.h"
#include "n3d_rasterizer.h"
#include "n3d_math.h"

namespace {

    template <typename type_t>
    type_t clamp(type_t lo, type_t in, type_t hi) {
        if (in < lo) return lo;
        if (in > hi) return hi;
        return in;
    }

    uint32_t rgb(float r, float g, float b) {

        r = clamp(0.f, r, 1.f);
        g = clamp(0.f, g, 1.f);
        b = clamp(0.f, b, 1.f);

        uint8_t r8 = uint8_t(r*255.f);
        uint8_t g8 = uint8_t(g*255.f);
        uint8_t b8 = uint8_t(b*255.f);

        return (r8 << 16) | (g8 << 8) | b8;
    }

} // namespace {}

void n3d_raster_reference_run(
    const n3d_rasterizer_t::state_t &s,
    const n3d_rasterizer_t::triangle_t &t,
    void * user) {

    typedef n3d_rasterizer_t::triangle_t::interp_t interp_t;

    const uint32_t pitch  = s.pitch_;
    const uint32_t width  = s.width_;
    const uint32_t height = s.height_;

    // barycentric interpolants
    vec3f_t bc_vy = { t.b0_.v_,  t.b1_.v_,  t.b2_.v_ };
    vec3f_t bc_sx = { t.b0_.sx_, t.b1_.sx_, t.b2_.sx_ };
    vec3f_t bc_sy = { t.b0_.sy_, t.b1_.sy_, t.b2_.sy_ };
    // shift to offset
    bc_vy += bc_sx * s.offset_.x;
    bc_vy += bc_sy * s.offset_.y;

    // colour interpolants
    vec3f_t cl_vy = { t.r_.v_,  t.g_.v_,  t.b_.v_ };
    vec3f_t cl_sx = { t.r_.sx_, t.g_.sx_, t.b_.sx_ };
    vec3f_t cl_sy = { t.r_.sy_, t.g_.sy_, t.b_.sy_ };
    // shift to offset
    cl_vy += cl_sx * s.offset_.x;
    cl_vy += cl_sy * s.offset_.y;

    // 1/w interpolants
    float w_vy = t.w_.v_;
    float w_sx = t.w_.sx_;
    float w_sy = t.w_.sy_;
    // shift to offset
    w_vy += w_sx * s.offset_.x;
    w_vy += w_sy * s.offset_.y;
    
    uint32_t * dst = s.color_;

    for (uint32_t y = 0; y < height; ++y) {

        vec3f_t bc_vx = bc_vy;
        vec3f_t cl_vx = cl_vy;
        float    w_vx = w_vy;

        for (uint32_t x = 0; x < width; ++x) {

            // check if inside triangle
            if (bc_vx.x >= 0.f && bc_vx.y >= 0.f && bc_vx.z >= 0.f) {

                // find fragment colour
                float r = cl_vx.x / w_vx;
                float g = cl_vx.y / w_vx;
                float b = cl_vx.z / w_vx;

                dst[x + y * pitch] = rgb(r, g, b);
            }

            bc_vx += bc_sx;
            cl_vx += cl_sx;
             w_vx += w_sx;
        }

        bc_vy += bc_sy;
        cl_vy += cl_sy;
         w_vy += w_sy;
    }
}
