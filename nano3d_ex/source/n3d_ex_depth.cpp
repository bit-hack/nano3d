#include "nano3d.h"
#include "source/n3d_math.h"

namespace {

template <typename type_t>
constexpr type_t clamp(type_t lo, type_t in, type_t hi)
{
    return (in < lo) ? lo : ((in > hi) ? hi : in);
}

uint32_t rgb(float r, float g, float b, float a)
{
    r = clamp(0.f, r, 1.f);
    g = clamp(0.f, g, 1.f);
    b = clamp(0.f, b, 1.f);

    const uint8_t r8 = uint8_t(r * 255.f);
    const uint8_t g8 = uint8_t(g * 255.f);
    const uint8_t b8 = uint8_t(b * 255.f);

    return (r8 << 16) | (g8 << 8) | b8;
}

} // namespace {}

void n3d_raster_depth_raster(
    const n3d_rasterizer_t::state_t& s,
    const n3d_rasterizer_t::triangle_t& t,
    void* user)
{
    const uint32_t pitch = s.pitch_;
    const uint32_t width = s.width_;
    const uint32_t height = s.height_;

    // barycentric interpolants
          vec3f_t bc_vy = { t.v_ [e_attr_b0], t.v_ [e_attr_b1], t.v_ [e_attr_b2] };
    const vec3f_t bc_sx = { t.sx_[e_attr_b0], t.sx_[e_attr_b1], t.sx_[e_attr_b2] };
    const vec3f_t bc_sy = { t.sy_[e_attr_b0], t.sy_[e_attr_b1], t.sy_[e_attr_b2] };

    // shift to offset
    bc_vy += bc_sx * s.offset_.x;
    bc_vy += bc_sy * s.offset_.y;

    // 1/w interpolants
          float w_vy = t.v_ [e_attr_w];
    const float w_sx = t.sx_[e_attr_w];
    const float w_sy = t.sy_[e_attr_w];

    // shift to offset
    w_vy += w_sx * s.offset_.x;
    w_vy += w_sy * s.offset_.y;

    uint32_t* dst = s.target_[n3d_target_pixel].uint32_;
    float* depth  = s.target_[n3d_target_depth].float_;

    // y axis
    for (uint32_t y = 0; y < height; ++y) {

        vec3f_t bc_vx = bc_vy;
        float w_vx = w_vy;

        // x axis
        for (uint32_t x = 0; x < width; ++x) {

            // check if inside triangle
            if (bc_vx.x >= 0.f && bc_vx.y >= 0.f && bc_vx.z >= 0.f) {

                // depth test (w buffering)
                if (w_vx > depth[x]) {

                    // update colour buffer
                    const float c = w_vx * 100.f;
                    dst[x] = rgb(c, c, c, c);

                    // update (w) depth buffer
                    depth[x] = w_vx;
                }
            }

            // step on x axis
            bc_vx += bc_sx;
            w_vx += w_sx;

        } // for (x axis)

        // step on y axis
        bc_vy += bc_sy;
        w_vy += w_sy;

        // step the buffers
        dst += pitch;
        depth += pitch;

    } // for (y axis)
}
