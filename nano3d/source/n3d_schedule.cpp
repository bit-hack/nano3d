// n3d_schedule.cpp:
//   schedule bins to worker threads

#include "n3d_schedule.h"
#include "n3d_atomic.h"
#include "n3d_bin.h"
#include <stdint.h>

struct n3d_worker_t : public n3d_thread_t {

    n3d_worker_t(n3d_schedule_t& schedule)
        : schedule_(schedule)
    {
    }

protected:
    n3d_schedule_t& schedule_;

    virtual void thread_func() override
    {
        n3d_bin_t* bin = schedule_.get_work(this);
        if (bin) {
            n3d_bin_process(bin);
        }
    }
};

void n3d_schedule_t::next_frame()
{
    reshuffle();

    n3d_assert(counter_ == 0);

    long new_val = (long)bins_.size();
    long old_val = n3d_atomic_xchg(counter_, new_val);

    n3d_assert(old_val == 0);

    n3d_atomic_inc(frame_num_);
}

void n3d_schedule_t::add(n3d_bin_t* bin, uint32_t num)
{
    num_bins_ += num;
    counter_ += num;

    for (uint32_t i = 0; i < num; ++i) {
        bins_.push_back(&bin[i]);
        bin[i].counter_ = &counter_;
    }
}

void n3d_schedule_t::reshuffle()
{
    n3d_assert(thread_.size()==num_threads_);

    if (num_threads_) {
        n3d_assert(thread_map_.get());
        n3d_assert(num_bins_);

        // todo: make some kind of priority heap here based on bin execution cost

        uint32_t stride = num_bins_/num_threads_;

        for (uint32_t i = 0; i<num_threads_; ++i) {
            thread_map_.get()[i] = i * stride;
        }
    }
}

bool n3d_schedule_t::start(const uint32_t max_threads)
{
    if (max_threads) {
        // todo: lets remove this in favour of thread_.size()
        num_threads_ = max_threads;

        // create the thread to bin mapping
        thread_map_.reset(new uint32_t[max_threads]);

        // create a bunch of worker threads
        for (uint32_t i = 0; i<max_threads; ++i) {
            thread_.push_back(new n3d_worker_t(*this));
        }

        // shuffle the thread to bin mapping
        reshuffle();

        // launch all of the workers
        for (auto & thread:thread_) {
            thread->start();
        }
    }
    else {
        //todo: do we need to reshuffle?
    }

    return true;
}

n3d_bin_t* n3d_schedule_t::get_work(n3d_thread_t* thread)
{
    // note: thread may be nullptr if its the main thread requesting work
//    n3d_assert(thread_map_.get());

    // if the counter is 0 we know there is no work to do
    if (counter_ == 0) {
        return nullptr;
    }

    // try at max all bins
    for (uint32_t i = 0; i < num_bins_; ++i) {

        n3d_bin_t* b = nullptr;

        // if we are a worker thread
        if (thread) {
            n3d_assert(num_threads_);
            uint32_t id = thread->get_id() % num_threads_;
            uint32_t& item = thread_map_.get()[id];
            b = bins_[(item++) % num_bins_];
        }
        // if we are the host thread
        else {
            b = bins_[i];
        }
        n3d_assert(b);

        // try to acquire this bin
        if (b->lock_.try_lock()) {

            // if this frame is complete
            if (b->frame_ > frame_num_) {
                // unlock and skip over
                b->lock_.unlock();
            } else {
                // bin needs processing
                return b;
            }
        }
    }

    // no work found
    return nullptr;
}

bool n3d_schedule_t::frame_is_done() const
{
    // if there are no more bins left to process
    return counter_ == 0;
}

void n3d_schedule_t::stop()
{
    // kill all worker threads
    for (auto& thread : thread_) {
        n3d_assert(thread);
        thread->stop();
    }
}
