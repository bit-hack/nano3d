#include "n3d_frame.h"
#include "n3d_bin.h"

namespace {

    // send a message to a single bin
    void send_one(n3d_bin_t * bin,
                  n3d_command_t & cmd) {

        n3d_assert(bin);
        while (!bin->pipe_.push(cmd))
            n3d_yield();
    }

    // send a message to all bins
    void send_all(n3d_frame_t * frame,
                  n3d_command_t & cmd) {

        n3d_assert(frame);
        for (uint32_t i=0; i<frame->num_bins_; ++i) {
            n3d_bin_t & bin = frame->bin_[i];
            send_one(&bin, cmd);
        }
    }

} // namespace {}

bool n3d_frame_create(n3d_frame_t * frame,
                      const n3d_framebuffer_t * framebuffer) {

    const uint32_t bin_w = 64;
    const uint32_t bin_h = 64;

    const uint32_t fb_width = framebuffer->width_;
    const uint32_t fb_height = framebuffer->height_;

    //(todo) dont simply round down and crop
    int bx = fb_width / bin_w;
    int by = fb_height / bin_h;
    int nbins = bx * by;
    n3d_assert(nbins > 0);

    frame->num_bins_ = nbins;

    frame->bin_ = new n3d_bin_t[nbins];
    n3d_assert(frame->bin_);

    float * depth = new float[fb_width * fb_height];
    n3d_assert(depth);
    frame->depth_ = depth;

    for (int i=0; i<nbins; ++i) {

        n3d_bin_t & bin = frame->bin_[i];

        bin.width_  = bin_w;
        bin.height_ = bin_h;
        bin.pitch_  = framebuffer->width_;

        uint32_t iox = (i % bx) * bin_w;
        uint32_t ioy = (i / bx) * bin_w;

        bin.offset_.x = float(iox);
        bin.offset_.y = float(ioy);

        // linear offset from origin [0,0]
        uint32_t fboffs = + iox + ioy * framebuffer->width_;

        bin.depth_ = fboffs + depth;
        bin.color_ = fboffs + framebuffer->pixels_;

        bin.rasterizer_ = nullptr;
        bin.texture_ = nullptr;

        bin.frame_ = 0;
    }

    return true;
}

void n3d_frame_free(n3d_frame_t * frame) {

    //(todo) kill all workers

    if (frame->depth_)
        delete [] frame->depth_;
    frame->depth_ = nullptr;

    if (frame->bin_)
        delete [] frame->bin_;
    frame->bin_ = nullptr;
}

void n3d_frame_send_triangle(
    n3d_frame_t * frame,
    n3d_rasterizer_t::triangle_t & triangle) {

    n3d_assert(frame);

    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_triangle;
    cmd.triangle_ = triangle;

    //(note) if we are pushing commands into a command queue we need to be
    //       sure that there is some way to consume those commands in case
    //       that the queue is full, as we would block forever.

    // itterate over all bins
    for (uint32_t i = 0; i < frame->num_bins_; ++i) {
        n3d_bin_t & bin = frame->bin_[i];

        // reject when triangle cant overlap the bin
        if (bin.offset_.x > (triangle.max_.x + 1.f))
            continue;
        if (bin.offset_.y > (triangle.max_.y + 1.f))
            continue;
        if ((bin.offset_.x + bin.width_) < triangle.min_.x)
            continue;
        if ((bin.offset_.y + bin.height_) < triangle.min_.y)
            continue;

        // send this triangle to the bin
        send_one(&bin, cmd);
    }
}

void n3d_frame_send_texture(n3d_frame_t * frame,
                            const n3d_texture_t * texture) {

    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_texture;
    cmd.texture_ = texture;
    send_all(frame, cmd);
}

void n3d_frame_send_rasterizer(n3d_frame_t * frame,
                               const n3d_rasterizer_t * rasterizer) {

    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_rasterizer;
    cmd.rasterizer_ = rasterizer;
    send_all(frame, cmd);
}

void n3d_frame_clear(n3d_frame_t * frame,
                     const uint32_t argb,
                     const float z) {

    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_clear;
    cmd.clear_.color_ = argb;
    cmd.clear_.depth_ = z;
    send_all(frame, cmd);
}

void n3d_frame_present(n3d_frame_t * frame) {

    n3d_command_t cmd;
    cmd.command_ = cmd.cmd_present;
    send_all(frame, cmd);
}
