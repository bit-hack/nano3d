#include "n3d_triangle.h"

namespace {

    // compute barycentric coordinates
    float orient2d(const vec4f_t & a, const vec4f_t & b) {

        return (b.x - a.x)*(-a.y) - (b.y - a.y)*(-a.x);
    }

    float min3( const float a, const float b, const float c ) {
        if (a < b) {
            return a < c ? a : c;
        }
        else {
            return b < c ? b : c;
        }
    }

    float max3( const float a, const float b, const float c ) {
        if (a > b) {
            return a > c ? a : c;
        }
        else {
            return b > c ? a : b;
        }
    }
};

bool n3d_prepare(
    n3d_rasterizer_t::triangle_t & tri,
    const n3d_vertex_t & v0,
    const n3d_vertex_t & v1,
    const n3d_vertex_t & v2) {

    const vec4f_t & vp0 = v0.p_;
    const vec4f_t & vp1 = v1.p_;
    const vec4f_t & vp2 = v2.p_;

    // the signed triangle area
    float t_area = (vp1.x - vp0.x)*(vp2.y - vp0.y)-
                   (vp2.x - vp0.x)*(vp1.y - vp0.y);

    // check for back face
    if (t_area <= 0.f)
        return false;

    // reciprocal of area for normalization
    t_area = 1.f / t_area;

    // find normalized barycentric coordinates
    tri.b0_.v_  = orient2d(vp1, vp2) * t_area;
    tri.b0_.sx_ = (vp1.y - vp2.y)    * t_area;
    tri.b0_.sy_ = (vp2.x - vp1.x)    * t_area;

    tri.b1_.v_  = orient2d(vp2, vp0) * t_area;
    tri.b1_.sx_ = (vp2.y - vp0.y)    * t_area;
    tri.b1_.sy_ = (vp0.x - vp2.x)    * t_area;

    tri.b2_.v_  = orient2d(vp0, vp1) * t_area;
    tri.b2_.sx_ = (vp0.y - vp1.y)    * t_area;
    tri.b2_.sy_ = (vp1.x - vp0.x)    * t_area;

    // calculate 1 / w for vertices
    float v0w = 1.f / v0.p_.w;
    float v1w = 1.f / v1.p_.w;
    float v2w = 1.f / v2.p_.w;

    //(todo) update all of this to use SSE

    // find triangle bounds
    tri.min_.x = min3( vp0.x, vp1.x, vp2.x );
    tri.min_.y = min3( vp0.y, vp1.y, vp2.y );
    tri.max_.x = max3( vp0.x, vp1.x, vp2.x ) + 1.f;
    tri.max_.y = max3( vp0.y, vp1.y, vp2.y ) + 1.f;

    // interplate 1 / w
    tri.w_.v_   = (tri.b0_.v_  * v0w) + (tri.b1_.v_  * v1w) + (tri.b2_.v_  * v2w);
    tri.w_.sx_  = (tri.b0_.sx_ * v0w) + (tri.b1_.sx_ * v1w) + (tri.b2_.sx_ * v2w);
    tri.w_.sy_  = (tri.b0_.sy_ * v0w) + (tri.b1_.sy_ * v1w) + (tri.b2_.sy_ * v2w);

    // interpolate r / w
    tri.r_.v_  = (tri.b0_.v_  * v0.c_.x * v0w) + (tri.b1_.v_  * v1.c_.x * v1w) + (tri.b2_.v_  * v2.c_.x * v2w);
    tri.r_.sx_ = (tri.b0_.sx_ * v0.c_.x * v0w) + (tri.b1_.sx_ * v1.c_.x * v1w) + (tri.b2_.sx_ * v2.c_.x * v2w);
    tri.r_.sy_ = (tri.b0_.sy_ * v0.c_.x * v0w) + (tri.b1_.sy_ * v1.c_.x * v1w) + (tri.b2_.sy_ * v2.c_.x * v2w);
                        
    // interpolate g / w
    tri.g_.v_  = (tri.b0_.v_  * v0.c_.y * v0w) + (tri.b1_.v_  * v1.c_.y * v1w) + (tri.b2_.v_  * v2.c_.y * v2w);
    tri.g_.sx_ = (tri.b0_.sx_ * v0.c_.y * v0w) + (tri.b1_.sx_ * v1.c_.y * v1w) + (tri.b2_.sx_ * v2.c_.y * v2w);
    tri.g_.sy_ = (tri.b0_.sy_ * v0.c_.y * v0w) + (tri.b1_.sy_ * v1.c_.y * v1w) + (tri.b2_.sy_ * v2.c_.y * v2w);
                        
    // interpolate b / w
    tri.b_.v_  = (tri.b0_.v_  * v0.c_.z * v0w) + (tri.b1_.v_  * v1.c_.z * v1w) + (tri.b2_.v_  * v2.c_.z * v2w);
    tri.b_.sx_ = (tri.b0_.sx_ * v0.c_.z * v0w) + (tri.b1_.sx_ * v1.c_.z * v1w) + (tri.b2_.sx_ * v2.c_.z * v2w);
    tri.b_.sy_ = (tri.b0_.sy_ * v0.c_.z * v0w) + (tri.b1_.sy_ * v1.c_.z * v1w) + (tri.b2_.sy_ * v2.c_.z * v2w);
                        
    // interpolate u / w
    tri.u_.v_  = (tri.b0_.v_  * v0.t_.x * v0w) + (tri.b1_.v_  * v1.t_.x * v1w) + (tri.b2_.v_  * v2.t_.x * v2w);
    tri.u_.sx_ = (tri.b0_.sx_ * v0.t_.x * v0w) + (tri.b1_.sx_ * v1.t_.x * v1w) + (tri.b2_.sx_ * v2.t_.x * v2w);
    tri.u_.sy_ = (tri.b0_.sy_ * v0.t_.x * v0w) + (tri.b1_.sy_ * v1.t_.x * v1w) + (tri.b2_.sy_ * v2.t_.x * v2w);
                        
    // interpolate v / w
    tri.v_.v_  = (tri.b0_.v_  * v0.t_.y * v0w) + (tri.b1_.v_  * v1.t_.y * v1w) + (tri.b2_.v_  * v2.t_.y * v2w);
    tri.v_.sx_ = (tri.b0_.sx_ * v0.t_.y * v0w) + (tri.b1_.sx_ * v1.t_.y * v1w) + (tri.b2_.sx_ * v2.t_.y * v2w);
    tri.v_.sy_ = (tri.b0_.sy_ * v0.t_.y * v0w) + (tri.b1_.sy_ * v1.t_.y * v1w) + (tri.b2_.sy_ * v2.t_.y * v2w);

    return true;
}
