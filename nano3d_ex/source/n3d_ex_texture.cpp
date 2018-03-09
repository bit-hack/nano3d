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

void n3d_raster_texture_raster(
    const n3d_rasterizer_t::state_t& s,
    const n3d_rasterizer_t::triangle_t& t,
    void* user)
{
    const aabb_t bound = get_bound(s, t);
    const uint32_t offsetx = s.offset_.x + bound.x0;
    const uint32_t offsety = s.offset_.y + bound.y0;
    const uint32_t pitch   = s.pitch_;

    const n3d_texture_t * tex = s.texure_;
    n3d_assert(tex && tex->texels_);

    // barycentric interpolants
          vec3f_t bc_vy = { t.v_ [e_attr_b0], t.v_ [e_attr_b1], t.v_ [e_attr_b2] };
    const vec3f_t bc_sx = { t.sx_[e_attr_b0], t.sx_[e_attr_b1], t.sx_[e_attr_b2] };
    const vec3f_t bc_sy = { t.sy_[e_attr_b0], t.sy_[e_attr_b1], t.sy_[e_attr_b2] };

    // shift to offset
    bc_vy += bc_sx * offsetx;
    bc_vy += bc_sy * offsety;

    // texture constants
    const uint32_t * texture = tex->texels_;
    const uint32_t usize = tex->width_;
    const uint32_t vsize = tex->height_;
    const uint32_t umask = usize - 1;
    const uint32_t vmask = vsize - 1;

    // uv interpolants
          vec2f_t uv_vy = { t.v_ [e_attr_u]*usize, t.v_ [e_attr_v]*vsize }; // origin
    const vec2f_t uv_sx = { t.sx_[e_attr_u]*usize, t.sx_[e_attr_v]*vsize }; // x step
    const vec2f_t uv_sy = { t.sy_[e_attr_u]*usize, t.sy_[e_attr_v]*vsize }; // y step
    // shift to offset
    uv_vy += uv_sx * offsetx;
    uv_vy += uv_sy * offsety;

    // 1/w interpolants
          float w_vy = t.v_ [e_attr_w];
    const float w_sx = t.sx_[e_attr_w];
    const float w_sy = t.sy_[e_attr_w];
    // shift to offset
    w_vy += w_sx * offsetx;
    w_vy += w_sy * offsety;

    // frame buffer surfaces
    uint32_t* dst = s.target_[n3d_target_pixel].uint32_;
    float* depth = s.target_[n3d_target_depth].float_;

    // pre step the buffers to x/y location
    dst   += pitch * bound.y0;
    depth += pitch * bound.y0;

    // y axis
    for (int32_t y = bound.y0; y < bound.y1; ++y) {

        vec3f_t bc_vx = bc_vy;
        vec2f_t uv_vx = uv_vy;
        float w_vx = w_vy;

        // x axis
        for (int32_t x = bound.x0; x < bound.x1; ++x) {

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
