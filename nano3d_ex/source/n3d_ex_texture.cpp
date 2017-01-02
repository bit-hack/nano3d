#include "nano3d.h"
#include "source/n3d_math.h"
#include "source/n3d_util.h"

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

void n3d_raster_texture_raster(
    const n3d_rasterizer_t::state_t& s,
    const n3d_rasterizer_t::triangle_t& t,
    void* user)
{
    const uint32_t pitch = s.pitch_;
    const uint32_t width = s.width_;
    const uint32_t height = s.height_;
    const n3d_texture_t * tex = s.texure_;
    n3d_assert(tex && tex->texels_);

    // barycentric interpolants
    vec3f_t bc_vy = { t.b0_.v_, t.b1_.v_, t.b2_.v_ }; // origin
    const vec3f_t bc_sx = { t.b0_.sx_, t.b1_.sx_, t.b2_.sx_ }; // x step
    const vec3f_t bc_sy = { t.b0_.sy_, t.b1_.sy_, t.b2_.sy_ }; // y step
    // shift to offset
    bc_vy += bc_sx * s.offset_.x;
    bc_vy += bc_sy * s.offset_.y;

    // texture constants
    const uint32_t * texture = tex->texels_;
    const uint32_t usize = tex->width_;
    const uint32_t vsize = tex->height_;
    const uint32_t umask = usize - 1;
    const uint32_t vmask = vsize - 1;

    // uv interpolants
          vec2f_t uv_vy = { t.u_.v_ *usize, t.v_.v_ *vsize }; // origin
    const vec2f_t uv_sx = { t.u_.sx_*usize, t.v_.sx_*vsize }; // x step
    const vec2f_t uv_sy = { t.u_.sy_*usize, t.v_.sy_*vsize }; // y step
    // shift to offset
    uv_vy += uv_sx * s.offset_.x;
    uv_vy += uv_sy * s.offset_.y;

    // 1/w interpolants
    float w_vy = t.w_.v_; // origin
    const float w_sx = t.w_.sx_; // x step
    const float w_sy = t.w_.sy_; // y step
    // shift to offset
    w_vy += w_sx * s.offset_.x;
    w_vy += w_sy * s.offset_.y;

    // frame buffer surfaces
    uint32_t* dst = s.target_[n3d_target_pixel].uint32_;
    float* depth = s.target_[n3d_target_depth].float_;

    // y axis
    for (uint32_t y = 0; y < height; ++y) {

        vec3f_t bc_vx = bc_vy;
        vec2f_t uv_vx = uv_vy;
        float w_vx = w_vy;

        // x axis
        for (uint32_t x = 0; x < width; ++x) {

            // check if inside triangle
            if (bc_vx.x >= 0.f && bc_vx.y >= 0.f && bc_vx.z >= 0.f) {

                // depth test (w buffering)
                if (w_vx > depth[x]) {

                    // find fragment colour
                    const float u = uv_vx.x / w_vx;
                    const float v = uv_vx.y / w_vx;

                    //todo: pull multiply out of inner loop
                    const int32_t ui = int32_t(u) & umask;
                    const int32_t vi = int32_t(v) & vmask;

                    // update colour buffer
                    dst[x] = texture[ui + vi*32];

                    // update (w) depth buffer
                    depth[x] = w_vx;
                }
            }

            // step on x axis
            bc_vx += bc_sx;
            uv_vx += uv_sx;
            w_vx += w_sx;

        } // for (x axis)

        // step on y axis
        bc_vy += bc_sy;
        uv_vy += uv_sy;
        w_vy += w_sy;

        // step the buffers
        dst += pitch;
        depth += pitch;

    } // for (y axis)
}
