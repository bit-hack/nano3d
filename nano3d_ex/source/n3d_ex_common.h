#pragma once
#include "nano3d.h"
#include "source/n3d_math.h"
#include "source/n3d_util.h"

namespace {

struct aabb_t {
    int32_t x0, x1;
    int32_t y0, y1;
};

inline aabb_t get_bound(
    const n3d_rasterizer_t::state_t& s,
    const n3d_rasterizer_t::triangle_t& t)
{
    // round for SSE alignment
    static constexpr int32_t c_mask = ~0xfu;
    return aabb_t{
        max2<int32_t>(0, t.min_.x - s.offset_.x) & c_mask,
        min2<int32_t>(s.width_, t.max_.x - s.offset_.x),
        max2<int32_t>(0, t.min_.y - s.offset_.y) & c_mask,
        min2<int32_t>(s.height_, t.max_.y - s.offset_.y),
    };
}

} // namespace {}
