// n3d_pipeline.cpp
//   implement the vertex processing pipeline and clipping stages

#include "n3d_pipeline.h"
#include "n3d_math.h"
#include "n3d_util.h"

namespace {

enum {
    OUTSIDE = 0, // both points outside
    INSIDE, // both points inside
    SPLIT_A_IN, // split and source inside
    SPLIT_B_IN, // split and dest inside
};

// v1, v2, t1, t2, vsplit, tsplit
uint32_t clip_near(const vec4f_t& v0,
    const vec4f_t& v1,
    const vec2f_t& t0,
    const vec2f_t& t1,
    const vec4f_t& c0,
    const vec4f_t& c1,
    vec4f_t& vsplit,
    vec2f_t& tsplit,
    vec4f_t& csplit)
{

    const float npv = 1.f;

    bool a_out = (v0.w <= npv);
    bool b_out = (v1.w <= npv);
    // fully out
    if (a_out & b_out)
        return OUTSIDE;
    // clipping
    if (a_out ^ b_out) {
        float a = v0.w;
        float b = v1.w;
        float c = 1.f;
        float ba = (b - a);
        float ca = (c - a);
        if (ba == 0.f) {
            vsplit = v0;
            tsplit = t0;
            csplit = c0;
            vsplit.w = 1.f;
            vsplit.z = -1.f;
            return SPLIT_A_IN + a_out;
        } else {
            float i = ca / ba;
            // find intersection point
            vsplit = n3d_lerp(i, v0, v1);
            tsplit = n3d_lerp(i, t0, t1);
            csplit = n3d_lerp(i, c0, c1);
            return SPLIT_A_IN + a_out;
        }
    }
    return INSIDE;
}

// check if a triangle is backfacing
bool is_backfacing(const vec4f_t& a, const vec4f_t& b, const vec4f_t& c)
{

    // test z componant of cross product
    const float cross = ((c.x - a.x) * (c.y - b.y) - (c.x - b.x) * (c.y - a.y));
    return ((*(uint32_t*)&cross) & 0x80000000) == 0;
}

} // namespace {}

// transform from world space into device coordinates
void n3d_transform(n3d_vertex_t* v, const uint32_t num_verts, const mat4f_t& m)
{

    //(todo) we can do this more efficiently

    for (uint32_t q = 0; q < num_verts; ++q) {
        vec4f_t& s = v[q].p_;
        n3d_transform(1, m, &s, &s);
    }
}

// this function only needs to clip triangles to the near plane however
// we can also reject triangles that are fully outside the frustum too.
void n3d_clip(n3d_vertex_t v[4], uint32_t& num_verts)
{

#if 0
    //NOTE: this check here has some precision problems and doesnt 1:1 match
    //      the barycentryic area check made later which can lead to cracks.

    // we can reject back faces here
    if (is_backfacing(v[0].p_, v[1].p_, v[2].p_)) {
        num_verts = 0;
        return;
    }
#endif

    // incremented when a frustum plane rejects a point
    uint32_t lx = 0, gx = 0;
    uint32_t ly = 0, gy = 0;
    uint32_t lz = 0, gz = 0;

    // check for rejection by any frustum planes
    for (uint32_t i = 0; i < 3; ++i) {
        const vec4f_t& p = v[i].p_;
        lx += (p.x < -p.w);
        gx += (p.x > p.w);
        ly += (p.y < -p.w);
        gy += (p.y > p.w);
        lz += (p.z < -p.w);
        gz += (p.z > p.w);
    }

    // all points rejected by a frustum plane so skip triangle
    if (lx == 3 || gx == 3 || ly == 3 || gy == 3 || lz == 3 || gz == 3) {
        num_verts = 0;
        return;
    }

    // triangle can be near plane clipped
    if (lz > 0) {

        n3d_vertex_t c[4];
        uint32_t cid = 0;

        for (uint32_t i = 0; i < 3; ++i) {

            const n3d_vertex_t& ca = v[i];
            const n3d_vertex_t& cb = v[(i + 1) % 3];
            n3d_vertex_t cm;

            uint32_t s = clip_near(
                ca.p_, cb.p_,
                ca.t_, cb.t_,
                ca.c_, cb.c_,
                cm.p_, cm.t_, cm.c_);

            if (s == SPLIT_A_IN || s == SPLIT_B_IN) {
                const float fep = 0.02f;
                n3d_assert(cm.p_.w > (1.f - fep) && cm.p_.w < (1.f + fep));
                n3d_assert(cm.p_.z > (-1.f - fep) && cm.p_.z < (-1.f + fep));
            }

            //(note): the starting vertex will be push in the last itteration
            switch (s) {
            case (OUTSIDE):
                break;
            case (SPLIT_A_IN):
                c[cid++] = cm;
                break;
            case (SPLIT_B_IN):
                c[cid++] = cm;
            case (INSIDE):
                c[cid++] = cb;
                break;
            default:
                n3d_assert(!"bad split id");
            }
        }

        n3d_assert(cid == 3 || cid == 4);
        num_verts = cid;

        for (uint32_t i = 0; i < num_verts; ++i)
            v[i] = c[i];
    }
}

// apply perspective divide
void n3d_w_divide(n3d_vertex_t v[4], const uint32_t num_verts)
{
    for (uint32_t i = 0; i < num_verts; ++i) {

        float iw = 1.f / v[i].p_.w;
        v[i].p_.x *= iw;
        v[i].p_.y *= iw;
        v[i].p_.z *= iw;
    }
}

// normalized device coordinates to device coordinates
void n3d_ndc_to_dc(n3d_vertex_t vert[4], const uint32_t num_verts, const vec2f_t sz)
{
    for (uint32_t i = 0; i < num_verts; ++i) {

        vec4f_t& p = vert[i].p_;
        p.x = (p.x + 1.f) * sz.x * .5f;
        p.y = (p.y + 1.f) * sz.y * .5f;
    }
}
