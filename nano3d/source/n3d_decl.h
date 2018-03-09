#pragma once

// n3d_decal:
// externaly facing declarations

#include <array>
#include <cstdint>

#include "n3d_config.h"

template <typename type_t>
struct vec2_t {
    type_t x, y;
};

template <typename type_t>
struct vec3_t {
    type_t x, y, z;
};

template <typename type_t>
struct vec4_t {
    type_t x, y, z, w;
};

template <typename type_t>
struct mat4_t {

    type_t& operator()(const uint32_t index)
    {
        return e[index];
    }

    type_t operator()(const uint32_t index) const
    {
        return e[index];
    }

    type_t& operator()(const uint32_t x, const uint32_t y)
    {
        return e[IX(x, y)];
    }

    type_t operator()(const uint32_t x, const uint32_t y) const
    {
        return e[IX(x, y)];
    }

protected:
    std::array<type_t, 16> e;

    static constexpr uint32_t IX(const uint32_t x, const uint32_t y)
    {
        return x + y * 4;
    }
};

typedef vec2_t<float> vec2f_t;
typedef vec3_t<float> vec3f_t;
typedef vec4_t<float> vec4f_t;
typedef mat4_t<float> mat4f_t;
typedef vec2_t<int32_t> vec2i_t;

enum n3d_attribute_t {
    e_attr_b0 = 0,
    e_attr_b1,
    e_attr_b2,
    e_attr_w,
    // custom attributes beyond this point (rgb, uv, ...)
    e_attr_custom = 4,
    // tex coordinates
    e_attr_u = 4,
    e_attr_v,
    // colour
    e_attr_r,
    e_attr_g,
    e_attr_b,
    // sentinel
    e_attr_count__ = 16
};

struct n3d_vertex_t {
    vec4f_t p_; // position
    static const size_t c_num_attrs = e_attr_count__ - e_attr_custom;
    std::array<float, c_num_attrs> attr_;
};
