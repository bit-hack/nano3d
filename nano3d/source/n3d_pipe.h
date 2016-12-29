#pragma once

#include "n3d_types.h"
#include "n3d_forward.h"
#include "n3d_atomic.h"
#include "n3d_thread.h"

// the n3d_pipe_t is how work items are sent to the individual bins of a frame.
// by design we can guarentee that only one thread will push messages, and only
// one thread at a time will try to pop messages.  we can use this to optimise
// the pipe design.

#define PIPE_TYPE_EXPERIMENTAL  1
#define PIPE_TYPE_SAFE          2

#define PIPE_TYPE               PIPE_TYPE_EXPERIMENTAL

#if (PIPE_TYPE == PIPE_TYPE_EXPERIMENTAL)
//note: this pipe is suitable for only ONE producer and ONE consumer there must
//      not be concurrent access to either end of this pipe.  it is my hope
//      that this pipe design will minimise the contention between producer and
//      the consumer to a minium.
template <typename type_t, uint32_t size_ = 1024>
struct n3d_pipe_t {

    static_assert(((size_-1)&size_) == 0, "size must be power of two");

    n3d_pipe_t()
        : head_(0)
        , tail_(0)
    {
        // set all locks to unlocked
        for (uint32_t i = 0; i < size_; ++i) {
            lock_[i] = c_unlocked;
        }
    }

    n3d_pipe_t(const n3d_pipe_t &) = delete;

    bool push(const type_t & in) {

        uint32_t i = head_ & mask_;
        uint8_t & lock = lock_[i];

        if (lock == c_unlocked) {
            data_[i] = in;
            lock = c_locked;
            ++head_;
            return true;
        }
        return false;
    }

    bool pop(type_t & out) {

        uint32_t i = tail_ & mask_;
        uint8_t & lock = lock_[i];

        if (lock == c_locked) {
            out = data_[i];
            lock = c_unlocked;
            ++tail_;
            return true;
        }
        return false;
    }

protected:

    static const uint8_t c_unlocked = 0;
    static const uint8_t c_locked   = 1;

    static const uint32_t mask_ = size_ - 1;

    type_t   data_[size_];
    uint8_t  lock_[size_];
    uint32_t head_, tail_;
};
#endif

#if (PIPE_TYPE == PIPE_TYPE_SAFE)
//note: this pipe was made while I was debugging a deadlock issue in the
//      schedule system.  this queue is supposed to be a little more safe
//      and conventional then the above one.  it may also be slower because
//      there can be contention between producer and consumer.
template <typename type_t, uint32_t size_ = 1024>
struct n3d_pipe_t {

    static_assert(((size_-1)&size_)==0, "size must be power of two");

    n3d_pipe_t()
        : head_(0)
        , tail_(0)
    {
    }

    n3d_pipe_t(const n3d_pipe_t &) = delete;

    bool push(const type_t & in) {

        if (lock_.try_lock()) {
            n3d_scope_spinlock_t guard(lock_, false);
            const long h =  head_    & mask_;
            const long t = (tail_-1) & mask_;
            if (h==t) {
                return false;
            }
            else {
                data_[h] = in;
                n3d_atomic_inc(head_);
                return true;
            }
        }
        return false;
    }

    bool pop(type_t & out) {
        
        if (lock_.try_lock()) {
            n3d_scope_spinlock_t guard(lock_, false);
            const long h = head_ & mask_;
            const long t = tail_ & mask_;
            if (h==t) {
                return false;
            }
            else {
                out = data_[t];
                n3d_atomic_inc(tail_);
                return true;
            }
        }
        return false;
    }

protected:

    static const uint8_t c_unlocked = 0;
    static const uint8_t c_locked = 1;

    static const uint32_t mask_ = size_-1;

    n3d_spinlock_t lock_;

    type_t       data_[size_];
    n3d_atomic_t head_, tail_;
};
#endif
