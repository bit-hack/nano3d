#pragma once

#include "n3d_types.h"
#include "n3d_forward.h"
#include "n3d_atomic.h"
#include "n3d_thread.h"

// a pipe suitable for only ONE producer and ONE consumer
// there must not be concurrent access to either end of this pipe
// it will allow one thread to send messages to another thread only.
template <typename type_t, uint32_t size_ = 1204>
struct n3d_pipe_t {

    static_assert(((size_-1)&size_) == 0, "size must be power of two");

    n3d_pipe_t()
        : head_(0)
        , tail_(0)
    {
        for (uint32_t i = 0; i < size_; ++i)
            signal_[i] = 0;
    }

    n3d_pipe_t(const n3d_pipe_t &) = delete;

    bool push(const type_t & in) {
        uint32_t i = head_ & mask_;
        if (signal_[i] == 0) {
            data_[i] = in;
            signal_[i] = 1;
            ++head_;
            return true;
        }
        return false;
    }

    bool pop(type_t & out) {
        uint32_t i = tail_ & mask_;
        if (signal_[i] == 1) {
            out = data_[i];
            signal_[i] = 0;
            ++tail_;
            return true;
        }
        return false;
    }

protected:
    
    static const uint32_t mask_ = size_ - 1;

    type_t data_[size_];
    int signal_[size_];
    uint32_t head_, tail_;
};
