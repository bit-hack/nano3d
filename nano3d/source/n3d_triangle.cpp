#include "n3d_util.h"
#include "n3d_triangle.h"

namespace {
// compute barycentric coordinates
constexpr float orient2d(const vec4f_t& a, const vec4f_t& b)
{
    return (b.x - a.x) * (-a.y) - (b.y - a.y) * (-a.x);
}
} // namespace

bool n3d_prepare(
    n3d_rasterizer_t::triangle_t& tri,
    const n3d_vertex_t& v0,
    const n3d_vertex_t& v1,
    const n3d_vertex_t& v2,
    const uint32_t flags)
{
    const vec4f_t& vp0 = v0.p_;
    const vec4f_t& vp1 = v1.p_;
    const vec4f_t& vp2 = v2.p_;

    // the signed triangle area
    const float t_area = (vp1.x - vp0.x) * (vp2.y - vp0.y) -
                         (vp2.x - vp0.x) * (vp1.y - vp0.y);

    // check for back face
    if (t_area <= 0.f)
        return false;
    // reciprocal of area for normalization
    const float rt_area = 1.f / t_area;

    // find normalized barycentric coordinates
    tri.v_ [e_attr_b0] = orient2d(vp1, vp2) * rt_area;
    tri.sx_[e_attr_b0] = (vp1.y - vp2.y)    * rt_area;
    tri.sy_[e_attr_b0] = (vp2.x - vp1.x)    * rt_area;

    tri.v_ [e_attr_b1] = orient2d(vp2, vp0) * rt_area;
    tri.sx_[e_attr_b1] = (vp2.y - vp0.y)    * rt_area;
    tri.sy_[e_attr_b1] = (vp0.x - vp2.x)    * rt_area;

    tri.v_ [e_attr_b2] = orient2d(vp0, vp1) * rt_area;
    tri.sx_[e_attr_b2] = (vp0.y - vp1.y)    * rt_area;
    tri.sy_[e_attr_b2] = (vp1.x - vp0.x)    * rt_area;

    // calculate 1 / w for vertices
    const float v0w = 1.f / v0.p_.w;
    const float v1w = 1.f / v1.p_.w;
    const float v2w = 1.f / v2.p_.w;

    //(todo) update all of this to use SSE

    // find triangle bounds
    tri.min_.x = min3(vp0.x, vp1.x, vp2.x);
    tri.min_.y = min3(vp0.y, vp1.y, vp2.y);
    tri.max_.x = max3(vp0.x, vp1.x, vp2.x) + 1.f;
    tri.max_.y = max3(vp0.y, vp1.y, vp2.y) + 1.f;

// barycenteric interpolate 1 param
#define BLERPW(B) ((tri.B[0] * v0w) + \
                   (tri.B[1] * v1w) + \
                   (tri.B[2] * v2w))

// barycentric interpolate 3 param
#define BLERPA(B, A) ((tri.B[0] * v0.attr_[A] * v0w) + \
                      (tri.B[1] * v1.attr_[A] * v1w) + \
                      (tri.B[2] * v2.attr_[A] * v2w))

    // interplate 1 / w
    tri.v_ [e_attr_w] = BLERPW(v_);
    tri.sx_[e_attr_w] = BLERPW(sx_);
    tri.sy_[e_attr_w] = BLERPW(sy_);

    for (uint32_t i = 0; i < v0.attr_.size(); ++i) {
      tri.v_ [e_attr_custom + i] = BLERPA(v_,  i);
      tri.sx_[e_attr_custom + i] = BLERPA(sx_, i);
      tri.sy_[e_attr_custom + i] = BLERPA(sy_, i);
    }

#undef BLERPW
#undef BLERPA
    return true;
}
