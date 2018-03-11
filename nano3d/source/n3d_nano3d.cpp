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

namespace {

static const size_t c_buffer_size = 1024;

struct vertex_array_t {

    // matrix transformation
    void transform(const uint32_t count, const mat4f_t& m)
    {
        for (uint32_t i = 0; i < count; ++i) {
            // load data
            float &x = x_[i], &y = y_[i], &z = z_[i], &w = w_[i];
            // transform vertices
            const float tx = x * m(0, 0) + y * m(1, 0) + z * m(2, 0) + w * m(3, 0);
            const float ty = x * m(0, 1) + y * m(1, 1) + z * m(2, 1) + w * m(3, 1);
            const float tz = x * m(0, 2) + y * m(1, 2) + z * m(2, 2) + w * m(3, 2);
            const float tw = x * m(0, 3) + y * m(1, 3) + z * m(2, 3) + w * m(3, 3);
            // save data
            (x = tx), (y = ty), (z = tz), (w = tw);
        }
    }

    // perspecive division
    void w_divide(const uint32_t count)
    {
        for (uint32_t i = 0; i < count; ++i) {
            x_[i] /= w_[i];
            y_[i] /= w_[i];
            z_[i] /= w_[i];
        }
    }

    // transform into normalized device coordinates
    void ndc_transform(const uint32_t count, const vec2f_t& sf)
    {
        for (uint32_t i = 0; i < count; ++i) {
            x_[i] = (x_[i] + 1.f) * sf.x;
            y_[i] = (y_[i] + 1.f) * sf.y;
        }
    }

    // pos
    std::array<float, c_buffer_size> x_;
    std::array<float, c_buffer_size> y_;
    std::array<float, c_buffer_size> z_;
    std::array<float, c_buffer_size> w_;
    // tex coords
    std::array<float, c_buffer_size> u_;
    std::array<float, c_buffer_size> v_;
    // colour
    std::array<float, c_buffer_size> r_;
    std::array<float, c_buffer_size> g_;
    std::array<float, c_buffer_size> b_;
};

} // namespace {}

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

    void update_comp_mat();

    n3d_result_e draw(
        uint32_t num_indices,
        const uint32_t* indices);

    // the bound vertex buffer
    n3d_vertex_buffer_t vertex_buffer_;

    // the target draw surface
    n3d_target_t target_;

    // the pipeline matrix stack
    std::array<mat4f_t, 2> matrix_;
    // composite matrix, which combines model view, projection and ndc transform
    mat4f_t comp_mat_;
    bool comp_mat_dirty_;

    n3d_framebuffer_t frame_;
    n3d_schedule_t schedule_;

    // vertex array is used as a staging area for vertices traveling through
    // the pipeline towards the rasterizers.
    vertex_array_t stage_;

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

void nano3d_t::detail_t::update_comp_mat()
{
    if (comp_mat_dirty_) {
        comp_mat_ = matrix_[n3d_model_view];
        n3d_multiply(comp_mat_, matrix_[n3d_projection]);
    }
    comp_mat_dirty_ = false;
}

n3d_result_e nano3d_t::detail_t::draw(
    uint32_t num_indices,
    const uint32_t* indices)
{
    n3d_vertex_buffer_t& vb = vertex_buffer_;

    // update the composite pipeline matrix
    update_comp_mat();

    // XXX: what does state_.buffer_ do again?
    int prep_flags = e_prepare_depth;
    prep_flags |= ((state_.buffer_ && state_.buffer_.get()->uv_) ? e_prepare_uv : 0);
    prep_flags |= ((state_.buffer_ && state_.buffer_.get()->rgb_) ? e_prepare_rgb : 0);

    const uint32_t *end = indices + num_indices;
    while (indices < end) {

        // todo: round to a multiple of three
        const uint32_t written = min2<uint32_t>(num_indices, (c_buffer_size / 3) * 3);

        for (uint32_t i = 0; i < written; ++i) {
            // shove vertices into staging array
            const vec3f_t& v = vb.pos_[indices[i]];
            stage_.x_[i] = v.x;
            stage_.y_[i] = v.y;
            stage_.z_[i] = v.z;
            stage_.w_[i] = 1.f;
        }
        // upload uv coordinates
        if (prep_flags & e_prepare_uv) {
            for (uint32_t i = 0; i < written; ++i) {
                const vec2f_t& v = vb.uv_[indices[i]];
                stage_.u_[i] = v.x;
                stage_.v_[i] = v.y;
            }
        }
        // upload uv coordinates
        if (prep_flags & e_prepare_rgb) {
            for (uint32_t i = 0; i < written; ++i) {
                const vec3f_t& v = vb.rgb_[indices[i]];
                stage_.r_[i] = v.x;
                stage_.g_[i] = v.y;
                stage_.b_[i] = v.z;
            }
        }

        // composite matrix combines modelview, projection and ndc transform
        stage_.transform(written, comp_mat_);

        // some kind of clipping must happen here

        // perspective division
        stage_.w_divide(written);

        const vec2f_t screen = { target_.width_ / 2, target_.height_ / 2 };
        stage_.ndc_transform(written, screen);

        // XXX: for now just convert back into AoS form :(
        //      replace me when n3d_prepare() is converted to SoA form
        for (uint32_t i = 2; i < written; i += 3) {
            std::array<n3d_vertex_t, 3> v;
            for (uint32_t k = 0; k < 3; ++k) {
              v[k].p_.x = stage_.x_[(i - 2) + k];
              v[k].p_.y = stage_.y_[(i - 2) + k];
              v[k].p_.z = stage_.z_[(i - 2) + k];
              v[k].p_.w = stage_.w_[(i - 2) + k];
            }
            n3d_rasterizer_t::triangle_t tri;
            if (!n3d_prepare(tri, v[0], v[1], v[2], prep_flags))
                continue;
            // send this triangle off for upload to the bins
            n3d_frame_send_triangle(&frame_, tri);
        }

        // advance along the index stream
        indices += written;
        num_indices -= written;
    }

    return n3d_result_e::n3d_sucess;
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
    d_.comp_mat_dirty_ = true;
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
#if NEW_PIPELINE
    return d_.draw(num_indices, indices);
#else // NEW_PIPELINE
    n3d_vertex_buffer_t& vb = d_.vertex_buffer_;

    // update the composite pipeline matrix
    // XXX: use dirty update
    d_.update_comp_mat();

    const uint32_t prep_flags = e_prepare_depth | ((d_.state_.buffer_ && d_.state_.buffer_.get()->uv_) ? e_prepare_uv : 0) | ((d_.state_.buffer_ && d_.state_.buffer_.get()->rgb_) ? e_prepare_rgb : 0);

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

        // transform by composite modelview*projection matrix
        n3d_transform(v, num, d_.comp_mat_);

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
#endif // NEW_PIPELINE
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
