#include <stdint.h>
#include "n3d_bin_man.h"
#include "n3d_atomic.h"

n3d_bin_man_t::pair_t::pair_t()
    : index_(0)
    , thread_(nullptr)
    , bin_()
{
}

void n3d_bin_man_t::add(n3d_bin_t *bin, n3d_thread_t *thread) {

    if (pair_.size() == 0) {
        pair_t p;
        p.thread_ = nullptr;
        p.index_ = 0;
        pair_.push_back(p);
    }
    {
        pair_t & p = pair_[0];
        p.bin_.push_back(bin);
    }
}

n3d_bin_t * n3d_bin_man_t::get_work(n3d_thread_t *thread, uint32_t frame) {

    while (true) {

        pair_t &p = pair_[0];
        if (p.bin_.size() == 0)
            return nullptr;

        long index = n3d_atomic_inc(p.index_) % p.bin_.size();

        n3d_bin_t & bin =
    }

    return p.bin_[index];
}
