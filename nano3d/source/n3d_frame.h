#pragma once

#include "nano3d.h"
#include "n3d_types.h"
#include "n3d_thread.h"

struct n3d_frame_t {

    uint32_t num_bins_;
    n3d_bin_t * bin_;

    //(todo) track the n3d_framebuffer_t here?

    // the depth buffer plane
    float * depth_;
};

bool n3d_frame_create(
    n3d_frame_t * frame,
    const n3d_framebuffer_t * framebuffer);

void n3d_frame_free(
    n3d_frame_t * frame);

void n3d_frame_send_triangle(
    n3d_frame_t * frame,
    n3d_rasterizer_t::triangle_t & triangle);

void n3d_frame_send_texture(
    n3d_frame_t * frame,
    const n3d_texture_t * texture);

void n3d_frame_send_rasterizer(
    n3d_frame_t * frame,
    const n3d_rasterizer_t * rasterizer);

void n3d_frame_send_user_data(
    n3d_frame_t * frame,
    const n3d_user_data_t * user_data);

void n3d_frame_clear(
    n3d_frame_t * frame,
    const uint32_t argb,
    const float z);

void n3d_frame_present(
    n3d_frame_t * frame);
