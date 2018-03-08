// #include "libs/float4.h"

#include "nano3d.h"
#include "source/n3d_math.h"
#include "source/n3d_util.h"

#include "n3d_ex_common.h"

namespace {

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

void n3d_raster_depth_raster_sse(
    const n3d_rasterizer_t::state_t& s,
    const n3d_rasterizer_t::triangle_t& t,
    void* user)
{
    // bin / triangle intersection boundary
    const aabb_t bound = get_bound(s, t);
    const uint32_t offsetx = s.offset_.x + bound.x0;
    const uint32_t offsety = s.offset_.y + bound.y0;
    const uint32_t pitch   = s.pitch_;

#if ATTRIB_ARRAY
    // barycentric interpolants
          vec3f_t bc_vy = { t.v_ [e_attr_b0], t.v_ [e_attr_b1], t.v_ [e_attr_b2] };
    const vec3f_t bc_sx = { t.sx_[e_attr_b0], t.sx_[e_attr_b1], t.sx_[e_attr_b2] };
    const vec3f_t bc_sy = { t.sy_[e_attr_b0], t.sy_[e_attr_b1], t.sy_[e_attr_b2] };
#else
          vec3f_t bc_vy = { t.b0_.v_,  t.b1_.v_,  t.b2_.v_  };
    const vec3f_t bc_sx = { t.b0_.sx_, t.b1_.sx_, t.b2_.sx_ };
    const vec3f_t bc_sy = { t.b0_.sy_, t.b1_.sy_, t.b2_.sy_ };
#endif
    // shift to offset
    bc_vy += bc_sx * offsetx;
    bc_vy += bc_sy * offsety;
    
#if ATTRIB_ARRAY
    // 1/w interpolants
          float w_vy = t.v_ [e_attr_w];
    const float w_sx = t.sx_[e_attr_w];
    const float w_sy = t.sy_[e_attr_w];
#else
          float w_vy = t.w_.v_;
    const float w_sx = t.w_.sx_;
    const float w_sy = t.w_.sy_;
#endif
    // shift to offset
    w_vy += w_sx * offsetx;
    w_vy += w_sy * offsety;

    // frame buffer targets
    uint32_t* dst = s.target_[n3d_target_pixel].uint32_;
    float* depth = s.target_[n3d_target_depth].float_;

    // pre step the buffers to x/y location
    dst   += pitch * bound.y0;
    depth += pitch * bound.y0;

    // y axis
    for (int32_t y = bound.y0; y < bound.y1; ++y) {

        // fresh variables to step along this scanline
        vec3f_t bc_vx = bc_vy;
        float w_vx = w_vy;

        // x axis
        // xxx: step over 4 pixels
        for (int32_t x = bound.x0; x < bound.x1; ++x) {

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
