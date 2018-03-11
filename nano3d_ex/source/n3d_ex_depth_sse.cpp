#include <intrin.h>

#include "nano3d.h"
#include "source/n3d_math.h"
#include "source/n3d_util.h"
#include "n3d_ex_common.h"

namespace {
template <typename in_t, typename out_t>
inline out_t bitcast(const in_t x)
{
    return *reinterpret_cast<const out_t*>(&x);
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

struct float4 {
    __m128 m;

    float4() = delete;

    explicit float4(__m128 v)
        : m(v)
    {
    }

    float4(float v)
        : m(_mm_set_ps1(v))
    {
    }

    explicit float4(float x, float y, float z, float w)
        : m(_mm_set_ps(w, z, y, x))
    {
    }

    void operator*=(const float& s)
    {
        m = _mm_mul_ps(m, _mm_set_ps1(s));
    }

    void operator+=(const float4& rhs)
    {
        m = _mm_add_ps(m, rhs.m);
    }

    void operator+=(const float& rhs) 
    {
        m = _mm_add_ps(m, _mm_set_ps1(rhs));
    }

    float operator[](uint32_t i) const
    {
      switch (i) {
      case 0: return bitcast<int, float>(_mm_extract_ps(m, 0));
      case 1: return bitcast<int, float>(_mm_extract_ps(m, 1));
      case 2: return bitcast<int, float>(_mm_extract_ps(m, 2));
      case 3: return bitcast<int, float>(_mm_extract_ps(m, 3));
      }
    }

    float x() const
    {
        return bitcast<int32_t, float>(_mm_extract_ps(m, 0));
    }

    float y() const
    {
        return bitcast<int32_t, float>(_mm_extract_ps(m, 1));
    }

    float z() const
    {
        return bitcast<int32_t, float>(_mm_extract_ps(m, 2));
    }

    float w() const
    {
        return bitcast<int32_t, float>(_mm_extract_ps(m, 3));
    }
};

float4 operator*(const float4& a, const float s)
{
    return float4{_mm_mul_ps(a.m, _mm_set_ps1(s))};
}

float4 operator*(const float4& a, const float4& b)
{
    return float4{_mm_mul_ps(a.m, b.m)};
}

float4 operator+(const float4& a, const float4& b)
{
    return float4{_mm_add_ps(a.m, b.m)};
}

float4 operator&(const float4& a, const float4& b)
{
    return float4{_mm_and_ps(a.m, b.m)};
}

float4 operator|(const float4& a, const float4& b)
{
    return float4{_mm_or_ps(a.m, b.m)};
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
        v_[i] += sx_[i] * float4{0.f, 1.f, 2.f, 3.f};
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

            const float4 b = vx_[0] | vx_[1] | vx_[2];

            // check if inside triangle
            if (b.x() >= 0.f) { kernel(x + 0, vx_[3].x(), dst, depth); }
            if (b.y() >= 0.f) { kernel(x + 1, vx_[3].y(), dst, depth); }
            if (b.z() >= 0.f) { kernel(x + 2, vx_[3].z(), dst, depth); }
            if (b.w() >= 0.f) { kernel(x + 3, vx_[3].w(), dst, depth); }

            // step on x axis
            vx_[0] += sx_[0];
            vx_[1] += sx_[1];
            vx_[2] += sx_[2];
            vx_[3] += sx_[3];

        } // for (x axis)

        // step on y axis
        v_[0] += sy_[0];
        v_[1] += sy_[1];
        v_[2] += sy_[2];
        v_[3] += sy_[3];

        // step the buffers
        dst   += pitch;
        depth += pitch;

    } // for (y axis)
}
