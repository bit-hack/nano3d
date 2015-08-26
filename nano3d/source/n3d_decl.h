#pragma once

#include <stdint.h>

template <typename type_t> 
struct vec2_t {
    union {
        type_t e[2];
        struct { type_t x, y; };
    };
};

template <typename type_t> 
struct vec3_t {
    union {
        type_t e[3];
        struct { type_t x, y, z; };
    };
};

template <typename type_t> 
struct vec4_t { 
    union {
        type_t e[4];
        struct { type_t x, y, z, w; };
    };
};

template <typename type_t> 
struct mat4_t { 
    type_t e[4 * 4]; 
};

typedef vec2_t<float>   vec2f_t;
typedef vec3_t<float>   vec3f_t;
typedef vec4_t<float>   vec4f_t;
typedef mat4_t<float>   mat4f_t;
typedef vec2_t<int32_t> vec2i_t;

struct n3d_vertex_t {

    vec4f_t p_;
    vec2f_t t_;
    vec3f_t c_;
};

namespace {

    static const float n3d_pi = 3.14159265359f;

};