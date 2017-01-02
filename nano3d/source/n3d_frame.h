#pragma once
#include <memory>
#include <vector>

#include "n3d_thread.h"
#include "n3d_types.h"
#include "nano3d.h"

// a n3d_frame_t manages the nano3d frame buffer and all of the bins that are
// produced from its subdivision. the frame system is also responsible for
// relaying commands from the frontend interface to the individual bin command
// queues.

struct n3d_frame_t {

    // bins assigned to this frame
    std::vector<std::unique_ptr<n3d_bin_t>> bin_;

    //todo: track the n3d_framebuffer_t here?

    // the depth buffer plane
    std::unique_ptr<float[]> depth_;
};

bool n3d_frame_create(
    n3d_frame_t* frame,
    const n3d_framebuffer_t* framebuffer);

void n3d_frame_free(
    n3d_frame_t* frame);

void n3d_frame_send_triangle(
    n3d_frame_t* frame,
    n3d_rasterizer_t::triangle_t& triangle);

void n3d_frame_send_texture(
    n3d_frame_t* frame,
    const n3d_texture_t* texture);

void n3d_frame_send_rasterizer(
    n3d_frame_t* frame,
    const n3d_rasterizer_t* rasterizer);

void n3d_frame_send_user_data(
    n3d_frame_t* frame,
    const n3d_user_data_t* user_data);

void n3d_frame_clear(
    n3d_frame_t* frame,
    const uint32_t argb,
    const float z);

void n3d_frame_present(
    n3d_frame_t* frame);
