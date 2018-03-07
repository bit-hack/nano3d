#pragma once

// n3d_decal:
// externaly facing declarations

#include <cstdint>
#include <array>

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

struct n3d_vertex_t {
    vec4f_t p_; // position
    vec2f_t t_; // texture
    vec4f_t c_; // rgb
};
