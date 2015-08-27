#include "../nano3d.h"
#include "n3d_util.h"
#include "n3d_triangle.h"
#include "n3d_pipeline.h"
#include "n3d_math.h"
#include "n3d_rasterizer.h"
#include "n3d_bin.h"
#include "n3d_bin_man.h"
#include "n3d_frame.h"

struct nano3d_t::detail_t {

    detail_t()
        : vertex_buffer_()
        , framebuffer_  ()
    {
        n3d_identity(matrix_[n3d_model_view]);
        n3d_identity(matrix_[n3d_projection]);
    }

    n3d_vertex_buffer_t   vertex_buffer_;

    n3d_framebuffer_t     framebuffer_;
    mat4f_t               matrix_[2];

    n3d_frame_t           frame_;
    n3d_bin_man_t         bin_man_;

    n3d_atomic_t          work_pending_;
};

nano3d_t::nano3d_t()
    : detail_(new nano3d_t::detail_t)
{
    n3d_assert(detail_);
}

nano3d_t::~nano3d_t() {
    n3d_assert(detail_);
    delete detail_;
}

n3d_result_e nano3d_t::start(n3d_framebuffer_t *f, uint32_t num_threads) {

    nano3d_t::detail_t  & d_ = *checked(detail_);
    d_.framebuffer_ = *f;

    if (!n3d_frame_create(&d_.frame_, f))
        return n3d_fail;

    // add the bins to the bin manager
    for (uint32_t i=0; i<d_.frame_.num_bins_; ++i) {
        d_.bin_man_.add(d_.frame_.bin_ + i, nullptr);
    }

    return n3d_sucess;
}

n3d_result_e nano3d_t::stop() {

    //(todo) send kill signal to worker threads, join
    return n3d_sucess;
}

n3d_result_e nano3d_t::bind(n3d_vertex_buffer_t *in) {

    nano3d_t::detail_t  & d_ = *checked(detail_);
    d_.vertex_buffer_ = *in;
    return n3d_sucess;
}

n3d_result_e nano3d_t::bind(n3d_rasterizer_t *in) {

    nano3d_t::detail_t  & d_ = *checked(detail_);
    n3d_frame_send_rasterizer(&d_.frame_, in);
    return n3d_sucess;
}

n3d_result_e nano3d_t::bind(n3d_texture_t *in) {

    nano3d_t::detail_t  & d_ = *checked(detail_);
    n3d_frame_send_texture(&d_.frame_, in);
    return n3d_sucess;
}

n3d_result_e nano3d_t::bind(mat4f_t *in, n3d_matrix_e slot) {

    nano3d_t::detail_t  & d_ = *checked(detail_);
    d_.matrix_[slot] = *in;
    return n3d_sucess;
}

n3d_result_e nano3d_t::draw(const uint32_t num_indices, const uint32_t * indices) {

    nano3d_t::detail_t  & d_ = *checked(detail_);
    n3d_vertex_buffer_t & vb = d_.vertex_buffer_;

    n3d_vertex_t v[4];

    n3d_assert((num_indices % 3) == 0);
    for (uint32_t i = 0; i < num_indices; i += 3) {

        const uint32_t i0 = indices[i + 0];
        const uint32_t i1 = indices[i + 1];
        const uint32_t i2 = indices[i + 2];

        vec2f_t uv = vec2(0.f, 0.f);
        vec3f_t rgb = vec3(1.f, 1.f, 1.f);

        v[0] = { vec4(vb.pos_[i0]), vb.uv_ ? vb.uv_[i0] : uv, vb.rgb_ ? vb.rgb_[i0] : rgb };
        v[1] = { vec4(vb.pos_[i1]), vb.uv_ ? vb.uv_[i1] : uv, vb.rgb_ ? vb.rgb_[i1] : rgb };
        v[2] = { vec4(vb.pos_[i2]), vb.uv_ ? vb.uv_[i2] : uv, vb.rgb_ ? vb.rgb_[i2] : rgb };

        // we have 3 vertices to start with but due to near plane clipping
        // this could increase to 4 vertices.  the following functions are
        // passed num by reference and it is free to change.
        uint32_t num = 3;

        // transform to world space
        n3d_transform(v, num, d_.matrix_[n3d_model_view]);

        // transform into homogeneous clip space
        n3d_transform(v, num, d_.matrix_[n3d_projection]);

        // clip triangle to the near plane
        n3d_clip(v, num);

        // apply perspective division
        n3d_w_divide(v, num);

        // transform from ndc to screen space
        vec2f_t sf = { float(d_.framebuffer_.width_), 
                       float(d_.framebuffer_.height_) };
        n3d_ndc_to_dc(v, num, sf);

        n3d_rasterizer_t::state_t state = {

            // render target pointers
            d_.framebuffer_.pixels_,
            nullptr,
            nullptr,
            // region to render
            d_.framebuffer_.width_,
            d_.framebuffer_.height_,
            // render target pitch
            d_.framebuffer_.width_
        };

        // feed triangles to bins
        n3d_assert(num == 3 || num == 4);
        for (uint32_t j = 2; j < num; ++j) {

            //(todo) allocate this from a global triangle list?
            //       some kind of ring allocator
            n3d_rasterizer_t::triangle_t tri;
            if (!n3d_prepare(tri, v[j], v[j-1], v[j-2]))
                continue;

            // send this triangle off for upload to the bins
            n3d_frame_send_triangle (&d_.frame_, tri);
        }
    }

    return n3d_sucess;
}

n3d_result_e nano3d_t::present() {

    nano3d_t::detail_t & d_ = *checked(detail_);
    n3d_frame_t & frame = d_.frame_;

    // send the present command
    n3d_frame_present(&frame);

    // ask the bin manager for work to do
    n3d_bin_t * bin = nullptr;
    while (!d_.bin_man_.frame_is_done()) {

        if (bin = d_.bin_man_.get_work(nullptr))
            n3d_bin_process(bin);
        else
            n3d_yield();
    }

    d_.bin_man_.next_frame();

    return n3d_sucess;
}

n3d_rasterizer_t * nano3d_t::rasterizer_new(n3d_rasterizer_e type) {

    n3d_rasterizer_t rast = {
        nullptr,
        nullptr
    };

    switch (type) {
    case (n3d_raster_reference) :
        rast.run_ = n3d_raster_reference_run;
        return new n3d_rasterizer_t(rast);

    default:
        n3d_assert(!"invalid n3d_rasterizer_e");
    }
    return nullptr;
}

void nano3d_t::rasterizer_delete(n3d_rasterizer_t * rast) {
    n3d_assert(rast);
    delete rast;
}

n3d_result_e nano3d_t::clear(uint32_t argb, float z) {
    nano3d_t::detail_t & d_ = *checked(detail_);
    n3d_frame_t & frame = d_.frame_;
    n3d_frame_clear(&frame, argb, z);
    return n3d_result_e::n3d_sucess;
}
