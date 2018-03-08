#include "nano3d.h"
#include "source/n3d_math.h"
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

struct interp_t {

    interp_t(float v, float sx, float sy, float x, float y)
        : sx_(sx)
        , sy_(sy)
        , vy_(v + sx * x + sy * y)
        , vx_(vy_)
    {
    }

    void step_x()
    {
        vx_ += sx_;
    }

    void step_y()
    {
        vy_ += sy_;
        vx_ = vy_; // reset x location
    }

    float operator () () const {
      return vx_;
    }

protected:
    const float sx_, sy_;
    float vx_, vy_;
};

} // namespace {}

void n3d_raster_rgb_raster(
    const n3d_rasterizer_t::state_t& s,
    const n3d_rasterizer_t::triangle_t& t,
    void* user)
{
#if 0
    // bin / triangle intersection boundary
    const aabb_t bound = get_bound(s, t);
    const float offsetx = s.offset_.x + bound.x0;
    const float offsety = s.offset_.y + bound.y0;
    const uint32_t pitch = s.pitch_;

    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
    // barycentric interpolants
          vec3f_t bc_vy = { t.b0_.v_,  t.b1_.v_,  t.b2_.v_  };        // initial
    const vec3f_t bc_sx = { t.b0_.sx_, t.b1_.sx_, t.b2_.sx_ };        // step
    const vec3f_t bc_sy = { t.b0_.sy_, t.b1_.sy_, t.b2_.sy_ };        // step
    // shift to offset
    bc_vy += bc_sx * offsetx;
    bc_vy += bc_sy * offsety;

    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
    // colour interpolants
          vec4f_t cl_vy = { t.r_.v_,  t.g_.v_,  t.b_.v_,  t.a_.v_  }; // initial
    const vec4f_t cl_sx = { t.r_.sx_, t.g_.sx_, t.b_.sx_, t.a_.sx_ }; // step
    const vec4f_t cl_sy = { t.r_.sy_, t.g_.sy_, t.b_.sy_, t.a_.sy_ }; // step
    // shift to offset
    cl_vy += cl_sx * offsetx;
    cl_vy += cl_sy * offsety;

    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
    // 1/w interpolant
          float w_vy = t.w_.v_;                                       // initial
    const float w_sx = t.w_.sx_;                                      // step
    const float w_sy = t.w_.sy_;                                      // step
    // shift to offset
    w_vy += w_sx * offsetx;
    w_vy += w_sy * offsety;

    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
    uint32_t* dst = s.target_[n3d_target_pixel].uint32_;
    float* depth = s.target_[n3d_target_depth].float_;

    // pre step the buffers to x/y location
    dst   += pitch * bound.y0;
    depth += pitch * bound.y0;

    // y axis
    for (int32_t y = bound.y0; y < bound.y1; ++y) {

        vec3f_t bc_vx = bc_vy;
        vec4f_t cl_vx = cl_vy;
        float w_vx = w_vy;

        // x axis
        for (int32_t x = bound.x0; x < bound.x1; ++x) {

            // check if inside triangle
            if (bc_vx.x >= 0.f && bc_vx.y >= 0.f && bc_vx.z >= 0.f) {

                // depth test (w buffering)
                if (w_vx > depth[x]) {

                    // find fragment colour
                    const float r = cl_vx.x / w_vx;
                    const float g = cl_vx.y / w_vx;
                    const float b = cl_vx.z / w_vx;
                    const float a = cl_vx.w / w_vx;

                    // update colour buffer
                    dst[x] = rgb(r, g, b, a);

                    // update (w) depth buffer
                    depth[x] = w_vx;
                }
            }

            // step on x axis
            bc_vx += bc_sx;
            cl_vx += cl_sx;
            w_vx += w_sx;

        } // for (x axis)

        // step on y axis
        bc_vy += bc_sy;
        cl_vy += cl_sy;
        w_vy += w_sy;

        // step the buffers
        dst += pitch;
        depth += pitch;

    } // for (y axis)
#endif
}
