#pragma once

#include "n3d_decl.h"

void n3d_identity(mat4f_t & m);

void n3d_frustum(
    mat4f_t & m,
    float left,
    float right,
    float bottom,
    float top,
    float near,
    float far);

void n3d_rotate(
    mat4f_t & m,
    float x,
    float y,
    float z
);

namespace {

    template <typename type_t>
    vec2_t<type_t> vec2(const type_t x, const type_t y) {
        vec2_t<type_t> o = { x, y };
        return o;
    }

    template <typename type_t>
    vec3_t<type_t> vec3(const type_t x, const type_t y, const type_t z) {
        vec3_t<type_t> o = { x, y, z };
        return o;
    }

    template <typename type_t>
    vec3_t<type_t> vec3(const vec4_t<type_t> & v) {
        const float iw = 1.f / v.w;
        vec3_t<type_t> o = { v.x * iw, v.y * iw, v.z * iw };
        return o;
    }

    template <typename type_t>
    vec4_t<type_t> vec4(vec3_t<type_t> & v) {
        vec4_t<type_t> o = { v.x, v.y, v.z, 1.f };
        return o;
    }

    template <typename type_t>
    vec4_t<type_t> vec4(const type_t x, const type_t y, const type_t z, const type_t w) {
        vec4_t<type_t> o = { x, y, z, w };
        return o;
    }

    void operator += (vec3f_t & a, const vec3f_t & b) {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
    }

    vec3f_t operator * (const vec3f_t & a, const float s) {
        return vec3(a.x*s, a.y*s, a.z*s);
    }

} // namespace {}
