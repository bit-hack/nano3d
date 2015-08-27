#pragma once

#include "n3d_forward.h"
#include "n3d_atomic.h"
#include <vector>

struct n3d_bin_man_t {

    n3d_bin_man_t()
        : bins_()
        , frame_num_(0)
        , counter_(0)
    {
    }

    void add( n3d_bin_t * bin, n3d_thread_t * thread );

    n3d_bin_t * get_work( n3d_thread_t * thread );

    void next_frame();

    bool frame_is_done() const;

protected:
    
    std::vector<n3d_bin_t*> bins_;

    uint32_t frame_num_;
    n3d_atomic_t counter_;
};
