#include "n3d_frame.h"
#include "n3d_bin.h"

bool n3d_frame_create(
    n3d_frame_t * frame,
    n3d_framebuffer_t * framebuffer) {

    const uint32_t bin_w = 64;
    const uint32_t bin_h = 64;

    const uint32_t fb_width = framebuffer->width_;
    const uint32_t fb_height = framebuffer->height_;

    //(todo) dont simply round down and crop
    int bx = fb_width / bin_w;
    int by = fb_height / bin_h;
    int nbins = bx * by;
    assert(nbins > 0);

    frame->num_bins_ = nbins;

    frame->bin_ = new n3d_bin_t[nbins];
    assert(frame->bin_);

    float * depth = new float[fb_width * fb_height];
    assert(depth);
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
    }

    return true;
}

void n3d_frame_free(
    n3d_frame_t * frame) {

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

    //(todo) send to all bins that this triangle aabb overlaps

    //(note) if we are pushing commands into a command queue we need to be
    //       sure that there is some way to consume those commands in case
    //       that the queue is full, as we would block forever.

    //(todo) pack as command and push to all bins
}

void n3d_frame_send_texture(
    n3d_frame_t * frame,
    n3d_texture_t * texture) {

    //(todo) pack as command and push to all bins
}

void n3d_frame_send_rasterizer(
    n3d_frame_t * frame,
    n3d_rasterizer_t * rasterizer) {

    //(todo) pack as command and push to all bins
}

void n3d_frame_clear(
    n3d_frame_t * frame) {

    //(todo) pack as command and push to all bins
}

void n3d_frame_present(
    n3d_frame_t * frame) {

    //(todo) pack as command and push to all bins
    //(todo) add the current thread as a worker
}
