#pragma once

#include "n3d_decl.h"

void n3d_identity(
    mat4f_t & m);

void n3d_frustum(
    mat4f_t & m,
    const float left,
    const float right,
    const float bottom,
    const float top,
    const float near,
    const float far);

void n3d_rotate(
    mat4f_t & m,
    const float a,
    const float b,
    const float c);

void n3d_translate(
    mat4f_t & m,
    const vec3f_t & p);

namespace {

    static const float n3d_pi = 3.14159265359f;

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

    void operator += (vec4f_t & a, const vec4f_t & b) {
        a.x += b.x;
        a.y += b.y;
        a.z += b.z;
        a.w += b.w;
    }

    vec4f_t operator * (const vec4f_t & a, const float s) {
        return vec4(a.x*s, a.y*s, a.z*s, a.w*s);
    }

    float n3d_lerp(float k, const float & a, const float & b) {
        return ((1.f - k) * a) + (k * b);
    }

    vec2f_t n3d_lerp(float k, const vec2f_t & a, const vec2f_t & b) {
        //(todo) SIMD lerp
        return vec2<float>(
            n3d_lerp(k, a.x, b.x),
            n3d_lerp(k, a.y, b.y)
            );
    }

    vec4f_t n3d_lerp(float k, const vec4f_t & a, const vec4f_t & b) {
        //(todo) SIMD lerp
        return vec4<float>(
            n3d_lerp(k, a.x, b.x),
            n3d_lerp(k, a.y, b.y),
            n3d_lerp(k, a.z, b.z),
            n3d_lerp(k, a.w, b.w)
            );
    }

} // namespace {}
