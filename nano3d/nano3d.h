#pragma once

#include "source/n3d_decl.h"

// vertex buffer definition
//      this can be bound to and n3d pipeline
struct n3d_vertex_buffer_t {

    // the number of vertices in this buffer
    uint32_t   num_;

    // raw vertex attribute arrays
    vec3f_t  * pos_;
    vec2f_t  * uv_;
    vec4f_t  * rgb_; //todo: rename to 'rgba_'
};

// texture definition
//      this can be bound to an n3d pipeline
struct n3d_texture_t {

    // texture size
    uint32_t   width_;
    uint32_t   height_;

    // raw texel data as 32bits per pixel
    uint32_t * texels_;
};

// framebuffer definition
//      this forms the render target for an n3d pipeline and
//      all output will be written here.
struct n3d_framebuffer_t {

    //todo: add pitch?
    uint32_t   width_;
    uint32_t   height_;

    // raw pixel data as 32bits per pixel
    uint32_t * pixels_;
};

// rasterizer definition
//      a rasterizer can be bound to an n3d pipeline.  it is
//      responsible for transforming a triangle setup into output
//      pixels in the render target.
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

        // triangle interpolation data
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

        // triangle bounds in screen space
        vec2f_t min_;
        vec2f_t max_;
    };

    // render target state
    struct state_t {

        //todo: add plane array (float/uint32)
        uint32_t      * color_;
        float         * depth_;

        // currently bound texture
        n3d_texture_t * texure_;

        // render target size
        uint32_t width_;
        uint32_t height_;

        // framebuffer pitch
        uint32_t pitch_;

        // bin offset from screen origin [0,0]
        vec2f_t offset_;
    };

    //todo: it would be nice to have a function like a vertex shader
    //      stage which can pass information from the raw vertices on
    //      to the raterizer itself.  for example, triangle normals.

//    void (*pre_run_)( ... , void * user);

    // user data passed to the rasterizer
    void *user_;

    // function pointer to the rasterizer
    void (*run_)(const state_t &, const triangle_t &, void * user);
};

// return codes for n3d api functions
enum n3d_result_e {

    n3d_fail  ,
    n3d_sucess,
};

// specifies the transform stages that can be bound to
enum n3d_matrix_e {

    n3d_projection,
    n3d_model_view,
};

struct nano3d_t {

    nano3d_t();
    ~nano3d_t();
    nano3d_t(const nano3d_t &) = delete;

    // description:
    //      initalize an n3d rendering context.  this must be called before
    //      any other functions.
    //
    // inputs:
    //      frame       - input framebuffer which is the render target
    //      num_planes  - number of additional colour planes to allocate.
    //                    each colour plane is 32bits per pixel.
    //      num_threads - number of worker threads to spawn for rendering.
    n3d_result_e start(n3d_framebuffer_t *frame,
                       uint32_t num_planes,
                       uint32_t num_threads);

    // description:
    //      shut down an n3d rendering context.
    n3d_result_e stop();

    // description:
    //      clear the n3d framebuffer colour and depth planes.
    n3d_result_e clear(uint32_t argb,
                       float z);

    // description:
    //      bind a vertex buffer to the n3d pipeline.
    n3d_result_e bind(n3d_vertex_buffer_t *);

    // description:
    //      bind a rasterizer to the n3d pipeline.
    n3d_result_e bind(n3d_rasterizer_t *);

    // description:
    //      bind a texture to the n3d pipeline.
    n3d_result_e bind(n3d_texture_t *);

    // description:
    //      bind a matrix to the n3d pipeline which will transform
    //      vertices from world space to ndc space.
    n3d_result_e bind(mat4f_t *,
                      n3d_matrix_e);

    // description:
    //      rasterize elements from the currently bound vertex buffer.
    n3d_result_e draw(const uint32_t num,
                      const uint32_t * indices);

    // description:
    //      flush the pipeline and make sure all output is present
    //      in the given rendertarget.
    n3d_result_e present();

protected:

    // hidden implementation
    struct detail_t;
    detail_t * detail_;
};
