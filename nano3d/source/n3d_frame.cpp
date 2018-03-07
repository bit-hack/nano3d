// n3d_frame.cpp
//   implement nano3d framebuffer and bin processor

#include <stdio.h>

#include "n3d_bin.h"
#include "n3d_frame.h"

namespace {

// send a message to a single bin
void send_one(
    n3d_bin_t* bin,
    n3d_command_t& cmd)
{
    n3d_assert(bin);
    while (!bin->pipe_.push(cmd)) {

        //todo: at this stage we could switch to the bin and give it some
        //      execution time until the bin is empty again or less full.

        //note: if there is no other thread to consume commands then at
        //      this stage we will deadlock.

        n3d_yield();
    }
}

// send a message to all bins
void send_all(
    n3d_framebuffer_t* frame,
    n3d_command_t& cmd)
{
    n3d_assert(frame);
    for (std::unique_ptr<n3d_bin_t>& bin : frame->bin_) {
        send_one(bin.get(), cmd);
    }
}

} // namespace {}

// create a new framebuffer
bool n3d_frame_create(
    n3d_framebuffer_t* frame,
    const n3d_target_t* framebuffer)
{
    const uint32_t bin_w = 64, bin_h = 64;

    const uint32_t fb_width = framebuffer->width_;
    const uint32_t fb_height = framebuffer->height_;

    //todo: dont simply round down and crop since we will get inactive borders
    //      around the framebuffer.

    const int bx = fb_width / bin_w;
    const int by = fb_height / bin_h;
    const int nbins = bx * by;
    n3d_assert(nbins > 0);

    // allocate a new depth buffer
    // xxx: this needs to be an aligned alloc
    frame->depth_.reset(new float[fb_width * fb_height]);
    float* depth = frame->depth_.get();
    n3d_assert(depth);

    // for each bin in this framebuffer
    auto& bins = frame->bin_;
    bins.reserve(nbins);
    for (int i = 0; i < nbins; ++i) {

        frame->bin_.emplace_back();
        frame->bin_.back().reset(new n3d_bin_t);
        n3d_assert(frame->bin_.back().get());
        n3d_bin_t& bin = *(frame->bin_.back().get());
        auto& state = bin.state_;

        // set bin id
        bin.id_ = i;

        // frame buffer dimensions
        state.width_  = bin_w;
        state.height_ = bin_h;
        state.pitch_  = framebuffer->width_;

        // bin integer screen space location
        uint32_t iox = (i % bx) * bin_w;
        uint32_t ioy = (i / bx) * bin_w;

        // bin screen space location
        state.offset_.x = float(iox);
        state.offset_.y = float(ioy);

        // linear bin offset from screen origin [0,0]
        uint32_t fboffs = +iox + ioy * framebuffer->width_;

        // render target state
        state.target_[n3d_target_depth].float_  = fboffs + depth;
        // xxx: check for aligned bin start
        state.target_[n3d_target_pixel].uint32_ = fboffs + framebuffer->pixels_;
        state.target_[n3d_target_aux_1].uint32_ = nullptr;
        state.target_[n3d_target_aux_2].uint32_ = nullptr;
        state.texure_ = nullptr;

        bin.rasterizer_ = nullptr;
        bin.frame_ = 0;
    }

    return true;
}

void n3d_frame_free(n3d_framebuffer_t* frame)
{
    n3d_assert(frame);
    frame->depth_.release();
    frame->bin_.clear();
}

void n3d_frame_send_triangle(
    n3d_framebuffer_t* frame,
    n3d_rasterizer_t::triangle_t& triangle)
{
    n3d_assert(frame);

    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_triangle;
    cmd.triangle_ = triangle;

    //note: if we are pushing commands into a command queue we need to be sure
    //      that there is some way to consume those commands in case that the
    //      queue is full, as we would block forever.

    // iterate over all bins
    for (uint32_t i = 0; i < frame->bin_.size(); ++i) {
        n3d_bin_t& bin = *(frame->bin_[i]);
        auto& state = bin.state_;

        // reject when triangle cant overlap the bin
        if (state.offset_.x > (triangle.max_.x + 1.f))
            continue;
        if (state.offset_.y > (triangle.max_.y + 1.f))
            continue;
        if ((state.offset_.x + state.width_) < triangle.min_.x)
            continue;
        if ((state.offset_.y + state.height_) < triangle.min_.y)
            continue;

        // send this triangle to the bin
        send_one(&bin, cmd);
    }
}

void n3d_frame_send_texture(
    n3d_framebuffer_t* frame,
    const n3d_texture_t* texture)
{
    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_texture;
    cmd.texture_ = texture;
    send_all(frame, cmd);
}

void n3d_frame_send_rasterizer(
    n3d_framebuffer_t* frame,
    const n3d_rasterizer_t* rasterizer)
{
    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_rasterizer;
    cmd.rasterizer_ = rasterizer;
    send_all(frame, cmd);
}

void n3d_frame_clear(
    n3d_framebuffer_t* frame,
    const uint32_t argb,
    const float z)
{
    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_clear;
    cmd.clear_.color_ = argb;
    cmd.clear_.depth_ = z;
    send_all(frame, cmd);
}

void n3d_frame_present(n3d_framebuffer_t* frame)
{
    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_present;
    send_all(frame, cmd);
}

void n3d_frame_send_user_data(
    n3d_framebuffer_t* frame,
    const n3d_user_data_t* user_data)
{
    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_user_data;
    cmd.user_data_ = *user_data;
}
