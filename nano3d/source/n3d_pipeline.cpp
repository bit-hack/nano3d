#if defined _MSC_VER
#include <intrin.h>
#endif
#include "n3d_pipeline.h"

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

void n3d_clip(n3d_vertex_t v[4], uint32_t & num_verts) {
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
