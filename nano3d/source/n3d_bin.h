#pragma once

#include "n3d_types.h"
#include "nano3d.h"
#include "n3d_thread.h"
#include "n3d_pipe.h"

struct n3d_command_t {

    enum {
        cmd_triangle    ,
        cmd_rasterizer  ,
        cmd_texture     ,
        cmd_present     ,
        cmd_clear       ,
        // custom user data to be passed to the rasterizer
        cmd_user        ,
    }
    command_;

    union
    {
        n3d_rasterizer_t::triangle_t triangle_;
        const n3d_rasterizer_t * rasterizer_;
        const n3d_texture_t * texture_;
        struct {
            uint32_t color_;
            float depth_;
        } clear_;
    };
};

typedef n3d_pipe_t<n3d_command_t, 1024> n3d_command_pipe_t;

struct n3d_bin_t {

    n3d_bin_t()
        : pipe_()
        , rasterizer_(nullptr)
#if 0
        , texture_(nullptr)
        , color_(nullptr)
        , depth_(nullptr)
#endif
        , counter_(nullptr)
    {
    }

    // locked when a thread is processing a bin
    n3d_spinlock_t lock_;

    // command pipe
    n3d_command_pipe_t pipe_;

    // pipeline state
    const n3d_rasterizer_t * rasterizer_;
#if 0
    const n3d_texture_t    * texture_;

    // bin offset from [0,0]
    vec2f_t   offset_;
#endif

    //(todo) move to this instead?
    n3d_rasterizer_t::state_t state_;

#if 0
    // render targets
    uint32_t *color_;
    float    *depth_;
    uint32_t  pitch_;

    // bin size
    uint32_t  width_;
    uint32_t  height_;
#endif

    // the current frame number
    uint32_t frame_;

    // 
    n3d_atomic_t * counter_;
};

void n3d_bin_process (
    n3d_bin_t * bin);
