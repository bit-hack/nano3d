// n3d_nano3dcpp
//   implement the nano3d api

#include <array>

#include "../nano3d.h"
#include "n3d_bin.h"
#include "n3d_frame.h"
#include "n3d_math.h"
#include "n3d_pipeline.h"
#include "n3d_schedule.h"
#include "n3d_triangle.h"
#include "n3d_util.h"

struct nano3d_t::detail_t {

    detail_t()
        : vertex_buffer_()
        , target_()
    {
        n3d_identity(matrix_[n3d_model_view]);
        n3d_identity(matrix_[n3d_projection]);
    }

    virtual ~detail_t()
    {
        n3d_frame_free(&frame_);
    }

    n3d_vertex_buffer_t vertex_buffer_;

    n3d_target_t target_;
    std::array<mat4f_t, 2> matrix_;

    n3d_framebuffer_t frame_;
    n3d_schedule_t schedule_;

    struct {
        valid_t<n3d_rasterizer_t> rasterizer_;
        valid_t<n3d_texture_t> texture_;
        valid_t<n3d_vertex_buffer_t> buffer_;
    } state_;
};

nano3d_t::nano3d_t()
    : detail_(new nano3d_t::detail_t)
{
    n3d_assert(detail_);
}

nano3d_t::~nano3d_t()
{
    stop();
    n3d_assert(detail_);
    delete detail_;
}

n3d_result_e nano3d_t::start(
    const n3d_target_t* f,
    const uint32_t num_planes,
    const uint32_t num_threads)
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    d_.target_ = *f;

    // create a frame buffer and all associated bins
    if (!n3d_frame_create(&d_.frame_, f))
        return n3d_fail;

    // add the bins to the bin manager
    for (std::unique_ptr<n3d_bin_t>& bin : d_.frame_.bin_) {
        d_.schedule_.add(bin.get(), 1);
    }

    // start the worker threads
    if (!d_.schedule_.start(num_threads)) {
        return n3d_fail;
    }

    return n3d_sucess;
}

n3d_result_e nano3d_t::stop()
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    d_.schedule_.stop();
    return n3d_sucess;
}

n3d_result_e nano3d_t::bind(
    const n3d_vertex_buffer_t* in)
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    d_.state_.buffer_ = *in;
    d_.vertex_buffer_ = *in;
    return n3d_sucess;
}

n3d_result_e nano3d_t::bind(
    const n3d_rasterizer_t* in)
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    d_.state_.rasterizer_ = *in;

    // todo: what happens when the n3d_rasterizer_t goes out of scope

    n3d_frame_send_rasterizer(&d_.frame_, in);
    return n3d_sucess;
}

n3d_result_e nano3d_t::bind(
    const n3d_texture_t* in)
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    d_.state_.texture_ = *in;

    // todo: need to create a local copy and do some reference counting here

    n3d_frame_send_texture(&d_.frame_, in);
    return n3d_sucess;
}

n3d_result_e nano3d_t::bind(
    const mat4f_t* in,
    const n3d_matrix_e slot)
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    d_.matrix_[slot] = *in;
    return n3d_sucess;
}

n3d_result_e nano3d_t::bind(
    const n3d_user_data_t* in)
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    n3d_frame_send_user_data(&d_.frame_, in);
    return n3d_sucess;
}

n3d_result_e nano3d_t::draw(
    const uint32_t num_indices,
    const uint32_t* indices)
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    n3d_vertex_buffer_t& vb = d_.vertex_buffer_;

    const int prep_flags = e_prepare_depth | ((d_.state_.buffer_ && d_.state_.buffer_.get()->uv_) ? e_prepare_uv : 0) | ((d_.state_.buffer_ && d_.state_.buffer_.get()->rgb_) ? e_prepare_rgb : 0);

    // space for 4 vertices since one more may be generated when clipping.
    n3d_vertex_t v[4];

    // indices should be multiple of 3 (triangle).
    n3d_assert((num_indices % 3) == 0);
    if (num_indices % 3)
        return n3d_fail;

    for (uint32_t i = 0; i < num_indices; i += 3) {

        //(todo) Batch transform instead of serial.
        //      1. Transform all pending vertices we can.
        //      2. Clip all triangles.
        //      3. Etc.

        const std::array<uint32_t, 3> ix = {
            indices[i + 0],
            indices[i + 2],
            indices[i + 1],
        };
        // upload position
        for (uint32_t j = 0; j < 3; ++j) {
            v[j].p_ = vec4(vb.pos_[ix[j]]);
        }
        // upload uv coordinates
        if (prep_flags & e_prepare_uv) {
            for (uint32_t j = 0; j < 3; ++j) {
                v[j].attr_[0] = vb.uv_[ix[j]].x;
                v[j].attr_[1] = vb.uv_[ix[j]].y;
            }
        }
        // upload rgb values
        if (prep_flags & e_prepare_rgb) {
            for (uint32_t j = 0; j < 3; ++j) {
                v[j].attr_[2] = vb.rgb_[ix[j]].x;
                v[j].attr_[3] = vb.rgb_[ix[j]].y;
                v[j].attr_[4] = vb.rgb_[ix[j]].z;
            }
        }

        // we have 3 vertices to start with but due to near plane clipping
        // this could increase to 4 vertices.  the following functions are
        // passed num by reference and it is free to change.
        uint32_t num = 3;

        // transform to world space
        n3d_transform(v, num, d_.matrix_[n3d_model_view]);

        // transform into homogeneous clip space
        n3d_transform(v, num, d_.matrix_[n3d_projection]);

#if 0
        // clip triangle to viewing frustum
        n3d_clip(v, num);
        if (num < 3)
            continue;
#endif

        // apply perspective division
        n3d_w_divide(v, num);

        // transform from ndc to screen space
        const vec2f_t sf = {
            float(d_.target_.width_),
            float(d_.target_.height_)
        };
        n3d_ndc_to_dc(v, num, sf);

        // feed triangles to bins
        n3d_assert(num == 3 || num == 4);
        for (uint32_t i = 2; i < num; ++i) {
            n3d_rasterizer_t::triangle_t tri;
            if (!n3d_prepare(tri, v[0], v[i], v[i - 1], prep_flags))
                continue;
            // send this triangle off for upload to the bins
            n3d_frame_send_triangle(&d_.frame_, tri);
        }
    }

    return n3d_sucess;
}

n3d_result_e nano3d_t::present()
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    n3d_framebuffer_t& frame = d_.frame_;

    // send the present command
    n3d_frame_present(&frame);

    // ask the bin manager for work to do
    n3d_bin_t* bin = nullptr;
    while (!d_.schedule_.frame_is_done()) {

        // if we are waiting then we can pitch in too
        if ((bin = d_.schedule_.get_work(nullptr))) {
            n3d_bin_process(bin);
        } else {
            n3d_yield();
        }
    }

    // move on to the next frame
    d_.schedule_.next_frame();

    return n3d_sucess;
}

n3d_result_e nano3d_t::clear(
    const uint32_t rgba,
    const float depth)
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    n3d_framebuffer_t& frame = d_.frame_;
    n3d_frame_clear(&frame, rgba, depth);
    return n3d_result_e::n3d_sucess;
}

n3d_result_e nano3d_t::project(
    const uint32_t num,
    const vec4f_t* in,
    vec4f_t* out)
{
    nano3d_t::detail_t& d_ = *checked(detail_);
    n3d_transform(num, d_.matrix_[n3d_matrix_e::n3d_model_view], in, out);
    return n3d_result_e::n3d_sucess;
}

n3d_result_e nano3d_t::unproject(
    const uint32_t num,
    const vec2f_t* in,
    vec3f_t* dir,
    vec3f_t* origin)
{
    //todo: implement
    return n3d_result_e::n3d_fail;
}
