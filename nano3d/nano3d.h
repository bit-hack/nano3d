#pragma once

#include "source/n3d_decl.h"

struct n3d_vertex_buffer_t {

    uint32_t   num_;
    vec3f_t  * pos_;
    vec2f_t  * uv_;
    vec4f_t  * rgb_;
};

struct n3d_texture_t {

    uint32_t   width_;
    uint32_t   height_;
    uint32_t * texels_;
};

struct n3d_framebuffer_t {

    uint32_t   width_;
    uint32_t   height_;
    uint32_t * pixels_;
};

struct n3d_rasterizer_t {

    // barycentric triangle
    struct triangle_t {

        // barycentric interpolant
        struct interp_t {

            //    . - - - - - value at origin
            //    |   . - - - change in x
            //    |   |    .- change in y
            //    |   |    |
            float v_, sx_, sy_;
        };

        interp_t b0_;   // b0
        interp_t b1_;   // b1
        interp_t b2_;   // b2

        interp_t w_;    // 1/w

        interp_t u_;    // u/w
        interp_t v_;    // v/w

        interp_t r_;    // red/w
        interp_t g_;    // green/w
        interp_t b_;    // blue/w
        interp_t a_;    // alpha/w

        // triangle bounds
        vec2f_t min_;
        vec2f_t max_;
    };

    // render target state
    struct state_t {

        uint32_t      * color_;
        float         * depth_;
        n3d_texture_t * texure_;
        // render target size
        uint32_t width_;
        uint32_t height_;
        // scanline width
        uint32_t pitch_;
        // offset from screen origin [0,0]
        vec2f_t offset_;
    };

    void *user_;

    void(*run_)(const state_t &, const triangle_t &, void * user);
};

enum n3d_result_e {

    n3d_fail  ,
    n3d_sucess,
};

enum n3d_matrix_e {

    n3d_projection,
    n3d_model_view,
};

struct nano3d_t {

    nano3d_t();
    ~nano3d_t();
    nano3d_t(const nano3d_t &) = delete;
    
    n3d_result_e start(n3d_framebuffer_t *, uint32_t num_threads);
    n3d_result_e stop();

    n3d_result_e clear(uint32_t argb, float z);

    n3d_result_e bind(n3d_vertex_buffer_t *);
    n3d_result_e bind(n3d_rasterizer_t *);
    n3d_result_e bind(n3d_texture_t *);
    n3d_result_e bind(mat4f_t *, n3d_matrix_e);

    n3d_result_e draw(const uint32_t num, const uint32_t * indices);
    n3d_result_e present();

protected:
    struct detail_t;
    detail_t * detail_;
};
