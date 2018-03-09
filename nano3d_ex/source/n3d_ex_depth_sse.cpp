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

struct float4 {

    float4() = delete;

    float4(float v)
        : e{ v, v, v, v }
    {
    }

    float4(float vx, float vy, float vz, float vw)
        : e{ vx, vy, vz, vw }
    {
    }

    void operator*=(const float& s)
    {
        e[0] *= s;
        e[1] *= s;
        e[2] *= s;
        e[3] *= s;
    }

    void operator+=(const float4& rhs)
    {
        e[0] += rhs.e[0];
        e[1] += rhs.e[1];
        e[2] += rhs.e[2];
        e[3] += rhs.e[3];
    }

    void operator+=(const float& rhs)
    {
        e[0] += rhs;
        e[1] += rhs;
        e[2] += rhs;
        e[3] += rhs;
    }

    float& operator[](uint32_t i)
    {
        return e[i];
    }

    float operator[](uint32_t i) const
    {
        return e[i];
    }

    std::array<float, 4> e;
};

float4 operator*(const float4& a, const float s)
{
    return float4{
        a.e[0] * s,
        a.e[1] * s,
        a.e[2] * s,
        a.e[3] * s };
}

float4 operator+(const float4& a, const float4& b)
{
    return float4{ 
        a.e[0] + b.e[0],
        a.e[1] + b.e[1],
        a.e[2] + b.e[2],
        a.e[3] + b.e[3] };
}

} // namespace {}

void n3d_raster_depth_raster_sse(
    const n3d_rasterizer_t::state_t& s,
    const n3d_rasterizer_t::triangle_t& t,
    void* user)
{
    // bin / triangle intersection boundary
    const aabb_t   bound   = get_bound(s, t);
    const uint32_t offsetx = s.offset_.x + bound.x0;
    const uint32_t offsety = s.offset_.y + bound.y0;
    const uint32_t pitch   = s.pitch_;

    static const uint32_t c_width = 4;
    static const uint32_t c_attrs = 4;
    std::array<float4, c_attrs> v_  = { t.v_ [0], t.v_ [1], t.v_ [2], t.v_ [3] };
    std::array<float4, c_attrs> sx_ = { t.sx_[0], t.sx_[1], t.sx_[2], t.sx_[3] };
    std::array<float,  c_attrs> sy_ = { t.sy_[0], t.sy_[1], t.sy_[2], t.sy_[3] };

    for (uint32_t i = 0; i < c_attrs; ++i) {
        // offset to correct coordinate
        v_[i] += sx_[i] * offsetx + sy_[i] * offsety;
        // offset by pixel ammount
        for (uint32_t j = 0; j < c_width; ++j) {
            v_[i][j] += sx_[i][j] * j;
        }
        // expand to 4x step
        sx_[i] *= c_width;
    }

    // frame buffer targets
    uint32_t* dst = s.target_[n3d_target_pixel].uint32_;
    float* depth = s.target_[n3d_target_depth].float_;

    // pre step the buffers to y location
    dst   += pitch * bound.y0;
    depth += pitch * bound.y0;

    // the pixel kernel
    const auto kernel = [](uint32_t x, float w, uint32_t* dst, float* depth)
    {
        // depth test (w buffering)
        if (w > depth[x]) {
            // update colour buffer
            const float c = w * 100.f;
            dst[x] = rgb(c, c, c, c);
            // update (w) depth buffer
            depth[x] = w;
        }
    };

    // y axis
    for (int32_t y = bound.y0; y < bound.y1; ++y) {

        // fresh variables to step along this scanline
        std::array<float4, c_attrs> vx_ = v_;

        // x axis
        for (int32_t x = bound.x0; x < bound.x1; x += c_width) {

            // check if inside triangle
            for (uint32_t j = 0; j < c_width; ++j) {
                if (vx_[0][j] >= 0.f && vx_[1][j] >= 0.f && vx_[2][j] >= 0.f) {
                    kernel(x + j, vx_[3][j], dst, depth);
                }
            }

            // step on x axis
            for (uint32_t i = 0; i < c_attrs; ++i) {
                vx_[i] += sx_[i];
            }

        } // for (x axis)

        // step on y axis
        for (uint32_t i = 0; i < c_attrs; ++i) {
            v_[i] += sy_[i];
        }

        // step the buffers
        dst   += pitch;
        depth += pitch;

    } // for (y axis)
}
