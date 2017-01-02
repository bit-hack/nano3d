#pragma once

#include <memory>
#include <vector>

#include "n3d_atomic.h"
#include "n3d_forward.h"
#include "n3d_thread.h"

// the n3d_schedule_t is responsible for managing worker threads and allocating
// bins to them for processing. the scheduler can perform load balancing to
// keep bin execution cost distributed across all threads while trying to
// maintain worker cache coherency. the current code is not quite there yet and
// will need to be looked at closely when optimising.

struct n3d_schedule_t {

    n3d_schedule_t()
        : bins_()
        , frame_num_(0)
        , counter_(0)
        , num_bins_(0)
        , num_threads_(0)
    {
    }

    // add a list of bins to the scheduler
    void add(n3d_bin_t* bin, uint32_t num);

    // return a bin that needs more work.  the thread parameter is a hint for
    // optimization and can be nullptr
    n3d_bin_t* get_work(n3d_thread_t* thread);

    void next_frame();

    // check if all bins have been processed for this frame
    bool frame_is_done() const;

    bool start(const uint32_t max_threads);

    void stop();

protected:
    // reshuffle the bin map
    void reshuffle();

    // todo: revise this an have a full thread -> bin map

    std::unique_ptr<uint32_t> thread_map_;

    std::vector<n3d_bin_t*> bins_;
    std::vector<n3d_thread_t*> thread_;

    n3d_atomic_t frame_num_;
    n3d_atomic_t counter_;

    // todo: replace these with std::vector.size()
    uint32_t num_bins_;
    uint32_t num_threads_;
};
