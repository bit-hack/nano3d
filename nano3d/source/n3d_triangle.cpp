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

#if ATTRIB_ARRAY
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
#else
    // find normalized barycentric coordinates
    tri.b0_.v_  = orient2d(vp1, vp2) * rt_area;
    tri.b0_.sx_ = (vp1.y - vp2.y) * rt_area;
    tri.b0_.sy_ = (vp2.x - vp1.x) * rt_area;

    tri.b1_.v_  = orient2d(vp2, vp0) * rt_area;
    tri.b1_.sx_ = (vp2.y - vp0.y) * rt_area;
    tri.b1_.sy_ = (vp0.x - vp2.x) * rt_area;

    tri.b2_.v_  = orient2d(vp0, vp1) * rt_area;
    tri.b2_.sx_ = (vp0.y - vp1.y) * rt_area;
    tri.b2_.sy_ = (vp1.x - vp0.x) * rt_area;
#endif

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
#define BLERP3(B, V, P) ((tri.B[0] * v0.V.P * v0w) + \
                         (tri.B[1] * v1.V.P * v1w) + \
                         (tri.B[2] * v2.V.P * v2w))

// barycentric interpolate 3 param
#define BLERPA(B, A) ((tri.B[0] * v0.attr_[A] * v0w) + \
                      (tri.B[1] * v1.attr_[A] * v1w) + \
                      (tri.B[2] * v2.attr_[A] * v2w))

#if ATTRIB_ARRAY
    // interplate 1 / w
    tri.v_ [e_attr_w] = BLERPW(v_);
    tri.sx_[e_attr_w] = BLERPW(sx_);
    tri.sy_[e_attr_w] = BLERPW(sy_);

    for (uint32_t i = 0; i < v0.attr_.size(); ++i) {
      tri.v_ [e_attr_custom + i] = BLERPA(v_,  i);
      tri.sx_[e_attr_custom + i] = BLERPA(sx_, i);
      tri.sy_[e_attr_custom + i] = BLERPA(sy_, i);
    }
#else
    if (flags & e_prepare_depth /* always need 1/w */) {
        // interplate 1 / w
        tri.w_.v_  = BLERPW(v_);
        tri.w_.sx_ = BLERPW(sx_);
        tri.w_.sy_ = BLERPW(sy_);
    }

    // XXX: convert u,v,r,g,b into an array of generic interpolants

    if (flags & e_prepare_rgb /* if raster.need_rgb */) {
        // interpolate r / w
        tri.r_.v_  = BLERP3(v_,  c_, x);
        tri.r_.sx_ = BLERP3(sx_, c_, x);
        tri.r_.sy_ = BLERP3(sy_, c_, x);
        // interpolate g / w
        tri.g_.v_  = BLERP3(v_,  c_, y);
        tri.g_.sx_ = BLERP3(sx_, c_, y);
        tri.g_.sy_ = BLERP3(sy_, c_, y);
        // interpolate b / w
        tri.b_.v_  = BLERP3(v_,  c_, z);
        tri.b_.sx_ = BLERP3(sx_, c_, z);
        tri.b_.sy_ = BLERP3(sy_, c_, z);
    }

    if (flags & e_prepare_uv /* if raster.need_uv */) {
        // interpolate u / w
        tri.u_.v_  = BLERP3(v_,  t_, x);
        tri.u_.sx_ = BLERP3(sx_, t_, x);
        tri.u_.sy_ = BLERP3(sy_, t_, x);
        // interpolate v / w
        tri.v_.v_  = BLERP3(v_,  t_, y);
        tri.v_.sx_ = BLERP3(sx_, t_, y);
        tri.v_.sy_ = BLERP3(sy_, t_, y);
    }
#endif

#undef BLERPW
#undef BLERP3
#undef BLERPA
    return true;
}
