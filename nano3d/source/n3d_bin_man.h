#pragma once

#include "n3d_forward.h"
#include "n3d_atomic.h"
#include <vector>

struct n3d_bin_man_t {

    void add( n3d_bin_t * bin, n3d_thread_t * thread );

    n3d_bin_t * get_work( n3d_thread_t * thread );

protected:

    struct pair_t {

        pair_t();

        n3d_atomic_t index_;
        n3d_thread_t * thread_;
        std::vector<n3d_bin_t*> bin_;
    };

    std::vector<pair_t> pair_;
};
