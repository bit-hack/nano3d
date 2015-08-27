#include <stdint.h>
#include "n3d_bin_man.h"
#include "n3d_atomic.h"
#include "n3d_bin.h"

void n3d_bin_man_t::next_frame() {

    ++frame_num_;
    counter_ += (long)bins_.size();
}

void n3d_bin_man_t::add(n3d_bin_t *bin, n3d_thread_t *thread) {

    bins_.push_back(bin);
    bin->counter_ = &counter_;
    n3d_atomic_inc(counter_);
}

n3d_bin_t * n3d_bin_man_t::get_work(n3d_thread_t *thread) {
    
    for (auto & b : bins_) {
        if (frame_num_ < b->frame_)
            continue;
        if (b->lock_.try_lock())
            return b;
    }
    return nullptr;
}

bool n3d_bin_man_t::frame_is_done() const {
    return counter_ == 0;
}
