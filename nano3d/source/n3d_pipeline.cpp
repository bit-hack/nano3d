#include "n3d_pipeline.h"
#include "n3d_util.h"

namespace {

bool point_in_clip_space(const vec4f_t & p) {

    if (p.x < -p.w || p.x > p.w) return false;
    if (p.y < -p.w || p.y > p.w) return false;
    if (p.z <  0.f || p.z > p.w) return false;
    return true;
}

template <typename T>
T lerp(float k, const T & a, const T & b) {
    return ((1.f - k) * a) + (k * b);
}

enum {
    OUTSIDE = 0, // both points outside
    INSIDE  ,    // both points inside
    SPLIT1  ,    // split and dest outside
    SPLIT2  ,    // split and dest inside
};

// v1, v2, t1, t2, vsplit, tsplit 
uint32_t clip_near( const vec4f_t & v0, 
                    const vec4f_t & v1, 
                    const vec2f_t & t0, 
                    const vec2f_t & t1, 
                    const vec4f_t & c0, 
                    const vec4f_t & c1, 
                    vec4f_t & vsplit, 
                    vec2f_t & tsplit,
                    vec4f_t & csplit ) {

    const float npv = 1.f;

    bool as = (v0.w <= npv);
    bool bs = (v1.w <= npv);
    // fully out
    if ( as & bs ) { 
        return OUTSIDE;
    }
    // clipping
    if ( as ^ bs ) {
        // difference between vertices
        float d = (v0.w - v1.w); 
        n3d_assert( d != .0f );
        // intersection ratio
        float i = (v0.w - npv) / d;
        // if clip occurs between vertices
//        if ( i >= 0.f && i <= 1.f ) {
            vsplit = lerp( i, v0, v1 );
            tsplit = lerp( i, t0, t1 );
            csplit = lerp( i, c0, c1 );
            return SPLIT1 + (!bs);
//        }
    }
    return INSIDE;
}

} // namespace {}

void n3d_transform(n3d_vertex_t * v, const uint32_t num_verts, const mat4f_t & m) {

    //(todo) faster matrix multiplication

    for (uint32_t q = 0; q < num_verts; ++q) {
        vec4f_t s = v[q].p_;
        vec4f_t d;
        for (uint32_t i = 0; i < 4; i++) {
            d.e[i] = 0.f;
            for (uint32_t j = 0; j < 4; j++)
                d.e[i] += m.e[j * 4 + i] * s.e[j];
        }
        v[q].p_ = d;
    }
}

// this function only needs to clip triangles to the near plane however
// we can also reject triangles that are fully outside the frustum too.
void n3d_clip(n3d_vertex_t v[4], uint32_t & num_verts) {

    uint32_t clip = 0;
    for (int i = 0; i < 3; i++)
        clip += point_in_clip_space(v[i].p_);
    if (clip == 3) {
        num_verts = 0;
        return;
    }

    if (clip > 0)
        num_verts = 0;

}

void n3d_w_divide(n3d_vertex_t v[4], const uint32_t num_verts) {

    for (uint32_t i = 0; i < num_verts; ++i) {

        float iw = 1.f / v[i].p_.w;
        v[i].p_.x *= iw;
        v[i].p_.y *= iw;
        v[i].p_.z *= iw;
    }
}

void n3d_ndc_to_dc(n3d_vertex_t vert[4], const uint32_t num_verts, const vec2f_t sz) {

    for (uint32_t i = 0; i < num_verts; ++i) {

        vec4f_t & p = vert[i].p_;
        p.x = (p.x + 1.f) * sz.x * .5f;
        p.y = (p.y + 1.f) * sz.y * .5f;
    }
}
