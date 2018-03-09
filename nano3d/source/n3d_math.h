#pragma once
#include <cmath>

#include "n3d_decl.h"

// here are some quick and dirty 3d math functions.  there should be some scope
// for optimisation here if these end up getting a lot of use.

void n3d_identity(
    mat4f_t& m);

/* OpenGL compatable perspective projection matrix */
void n3d_frustum(
    mat4f_t& m,
    const float left,
    const float right,
    const float bottom,
    const float top,
    const float near,
    const float far);

/* generate a rotation matrix */
void n3d_rotate(
    mat4f_t& m,
    const float a,
    const float b,
    const float c);

/* set a matrix translation */
void n3d_translate(
    mat4f_t& m,
    const vec3f_t& p);

/* transform an array of vectors by a matrix */
void n3d_transform(
    const uint32_t num_verts,
    const mat4f_t& m,
    const vec4f_t* in,
    vec4f_t* out);

/* matix inversion */
bool n3d_invert(
    mat4f_t& mat);

/* matrix transpose */
void n3d_transpose(
    mat4f_t& mat);

/* matrix multiplication */
void n3d_multiply(
    mat4f_t& mat,
    const mat4f_t& rhs);

/* glRotatef */
void n3d_rotatef(mat4f_t &mat,
                 float angle,
                 float x,
                 float y,
                 float z);

/* glScalef */
void n3d_scalef(mat4f_t &mat,
                float x,
                float y,
                float z);

/* glTranslatef */
void n3d_translatef(mat4f_t &mat,
                    float x,
                    float y,
                    float z);

namespace {

static constexpr float n3d_pi = 3.14159265359f;
static constexpr float n3d_pi2 = n3d_pi * 2.f;

template <typename type_t>
inline vec2_t<type_t> vec2(
    const type_t x,
    const type_t y)
{
    return vec2_t<type_t>{ x, y };
}

template <typename type_t>
inline vec3_t<type_t> vec3(
    const type_t x,
    const type_t y,
    const type_t z)
{
    return vec3_t<type_t>{ x, y, z };
}

template <typename type_t>
inline vec4_t<type_t> vec4(
    const type_t x,
    const type_t y,
    const type_t z,
    const type_t w)
{
    return vec4_t<type_t>{ x, y, z, w };
}

template <typename type_t>
inline vec3_t<type_t> vec3(const vec4_t<type_t>& v)
{
    const float iw = 1.f / v.w;
    return vec3_t<type_t>{ v.x * iw, v.y * iw, v.z * iw };
}

template <typename type_t>
inline vec4_t<type_t> vec4(const vec3_t<type_t>& v)
{
    return vec4_t<type_t>{ v.x, v.y, v.z, 1.f };
}

inline vec2f_t operator*(
    const vec2f_t& a,
    const float s)
{
    return vec2f_t{ a.x * s, a.y * s };
}

inline vec3f_t operator*(
    const vec3f_t& a,
    const float s)
{
    return vec3f_t{ a.x * s, a.y * s, a.z * s };
}

inline vec4f_t operator*(
    const vec4f_t& a,
    const float s)
{
    return vec4f_t{ a.x * s, a.y * s, a.z * s, a.w * s };
}

inline void operator+=(
    vec2f_t& a,
    const vec2f_t& b)
{
    a.x += b.x;
    a.y += b.y;
}

inline void operator+=(
    vec3f_t& a,
    const vec3f_t& b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
}

inline void operator+=(
    vec4f_t& a,
    const vec4f_t& b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    a.w += b.w;
}

inline void operator-=(
    vec2f_t& a,
    const vec2f_t& b)
{
    a.x -= b.x;
    a.y -= b.y;
}

inline void operator-=(
    vec3f_t& a,
    const vec3f_t& b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
}

inline void operator-=(
    vec4f_t& a,
    const vec4f_t& b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    a.w -= b.w;
}

inline float n3d_lerp(
    float k,
    const float a,
    const float b)
{
//  return (1.f - k) * a + k * b;
    return a + (b - a) * k;
}

inline vec2f_t n3d_lerp(
    float k,
    const vec2f_t& a,
    const vec2f_t& b)
{
    return vec2f_t{
        n3d_lerp(k, a.x, b.x),
        n3d_lerp(k, a.y, b.y)
    };
}

inline vec3f_t n3d_lerp(
    float k,
    const vec3f_t& a,
    const vec3f_t& b)
{
    return vec3f_t{
        n3d_lerp(k, a.x, b.x),
        n3d_lerp(k, a.y, b.y),
        n3d_lerp(k, a.z, b.z)
    };
}

inline vec4f_t n3d_lerp(
    float k,
    const vec4f_t& a,
    const vec4f_t& b)
{
    return vec4f_t{
        n3d_lerp(k, a.x, b.x),
        n3d_lerp(k, a.y, b.y),
        n3d_lerp(k, a.z, b.z),
        n3d_lerp(k, a.w, b.w)
    };
}

inline float n3d_dot(const vec3f_t& a, const vec3f_t& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float n3d_dot(const vec4f_t& a, const vec4f_t& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline vec3f_t n3d_normalize(const vec3f_t& v)
{
    const float sdist = n3d_dot(v, v);
    const float dist = sqrtf(sdist);
    return v * (1.f / dist);
}

inline vec4f_t n3d_normalize(const vec4f_t& v)
{
    const float sdist = n3d_dot(v, v);
    const float dist = sqrtf(sdist);
    return v * (1.f / dist);
}

inline vec3f_t n3d_cross(const vec3f_t& a, const vec3f_t& b)
{
    return vec3f_t{
        a.y * b.z - b.y * a.z,
        a.z * b.x - b.z * a.x,
        a.x * b.y - b.x * a.y,
    };
}

} // namespace {}
