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
    vec4f_t  * rgba_;
};

// texture definition
//      this can be bound to an n3d pipeline
struct n3d_texture_t {

    // texture size
    uint32_t   width_;
    uint32_t   height_;

    // raw texel data as 32bits per pixel ARGB
    uint32_t * texels_;
};

// frame buffer definition
//      this forms the render target for an n3d pipeline and
//      all output will be written here.
struct n3d_framebuffer_t {

    //todo: add pitch?
    uint32_t   width_;
    uint32_t   height_;

    // raw pixel data as 32bits per pixel
    uint32_t * pixels_;
};

// user data definition
//      user data can be passed synchronously to the rasterizer
struct n3d_user_data_t {

    union {
        uint8_t  uint8_[32];
        uint32_t uint32_[4];
        vec4f_t  vec4f_;
    };
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

        n3d_user_data_t user_;
    };

    // generic render target
    union target_t {
        uint32_t * uint32_;
        float    * float_;
    };

    // render target state
    struct state_t {

        // render targets
        target_t target_[4];

        // currently bound texture
        const n3d_texture_t * texure_;

        // render target size
        uint32_t width_;
        uint32_t height_;

        // frame buffer pitch
        uint32_t pitch_;

        // bin offset from screen origin [0,0]
        vec2f_t offset_;
    };

    // user data passed to the rasterizer
    void *user_;

    // triangle rasterizer
    void (*raster_proc_)(const state_t & state,
                         const triangle_t & triangle,
                         void * user);
};

// return codes for n3d api functions
enum n3d_result_e {

    n3d_fail,
    n3d_sucess,
};

// specifies the transform stages that can be bound to
enum n3d_matrix_e {

    n3d_projection,
    n3d_model_view,
};

// rasterizer target buffers
enum n3d_target_e {

    n3d_target_pixel,
    n3d_target_depth,
    n3d_target_aux_1,
    n3d_target_aux_2,
};

// primitive stitching mode
enum n3d_primitive_e {

    n3d_prim_tri,
    n3d_prim_tri_stip,
    n3d_prim_tri_fan,
};

struct nano3d_t {

    nano3d_t();
    nano3d_t(const nano3d_t &) = delete;
    ~nano3d_t();

    // description:
    //      initialize an n3d rendering context. this must be called before
    //      any other functions.
    //
    // inputs:
    //      frame       - input frame buffer which is the render target
    //      num_planes  - number of additional colour planes to allocate.
    //                    each colour plane is 32bits per pixel.
    //      num_threads - number of worker threads to spawn for rendering.
    n3d_result_e start(const n3d_framebuffer_t *frame,
                       const uint32_t num_planes,
                       const uint32_t num_threads);

    // description:
    //      shut down an n3d rendering context.
    //
    // inputs:
    //      n/a
    n3d_result_e stop();

    // description:
    //      clear the n3d frame buffer colour and depth planes.
    //
    // inputs:
    //      rgba        - colour to clear frame buffer to
    //      depth       - value to clear depth buffer to
    n3d_result_e clear(const uint32_t rgba,
                       const float depth);

    // description:
    //      bind a vertex buffer to the n3d pipeline.
    //
    // inputs:
    //      buffer      - buffer to bind to pipeline
    n3d_result_e bind(const n3d_vertex_buffer_t *buffer);

    // description:
    //      bind a rasterizer to the n3d pipeline.
    //
    // inputs:
    //      rasterizer  - rasterizer to bind to pipeline
    n3d_result_e bind(const n3d_rasterizer_t * rasterizer);

    // description:
    //      bind a texture to the n3d pipeline.
    //
    // inputs:
    //      texture     - texture to bind to pipeline
    n3d_result_e bind(const n3d_texture_t * texture);

    // description:
    //      bind a matrix to the n3d pipeline which will transform
    //      vertices from world space to ndc space.
    //
    // inputs:
    //      matrix      - 4x4 matrix to be bound
    //      slot        - pipeline slot to bind matrix to
    n3d_result_e bind(const mat4f_t * matrix,
                      const n3d_matrix_e slot);

    // description:
    //      bin a set of user data to the current n3d pipeline.
    //
    // input:
    //      in          - the user data to be 
    // 
    n3d_result_e bind(const n3d_user_data_t * in);

    // description:
    //      rasterize elements from the currently bound vertex buffer.
    //
    // inputs:
    //      num         - number of raw indices to process
    //      indices     - stream of vertex indices
    n3d_result_e draw(const uint32_t num,
                      const uint32_t * indices
                      /* const uint32_t mode */);

    // description:
    //      flush the pipeline and make sure all output is present
    //      in the given render target.
    //
    n3d_result_e present();

    // description:
    //      project a point from world space to screen space.
    //
    // input:
    //      num         - number of points to project
    //      input       - input world space point
    //      output      - output screen space point
    n3d_result_e n3d_project(const uint32_t num,
                             const vec4f_t * in,
                             vec4f_t * out);

    // description:
    //      project a point from screen space to world space.
    //
    // input:
    //      num         - number of points to un project
    //      in          - input screen space points
    //      dir         - output direction vector from origin
    //      origin      - world space camera position
    n3d_result_e n3d_unproject(const uint32_t num,
                               const vec2f_t * in,
                               vec3f_t * dir,
                               vec3f_t * origin);

protected:

    // hidden implementation
    struct detail_t;
    detail_t * detail_;
};
