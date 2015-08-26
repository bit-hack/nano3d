#pragma once

#include "n3d_types.h"
#include "n3d_forward.h"
#include "n3d_atomic.h"
#include "n3d_thread.h"

template <typename type_t, uint32_t size_ = 1204 * 8>
struct n3d_pipe_t {

    n3d_pipe_t()
        : head_(0)
        , tail_(0)
    {
        for (uint32_t i=0; i<size_; ++i)
            atom_[i] = 0;
    }

    n3d_pipe_t(const n3d_pipe_t &) = delete;

    bool push(const n3d_command_t &) {

        //(todo) implement this function
        return false;

        long slot = n3d_atomic_inc(head_);
        while (n3d_atomic_xchg(atom_[slot], 1)!=0)
            n3d_yield ();

        return true;
    }

    bool pop(n3d_command_t &) {

        return false;
    }

protected:

    type_t cmd_[size_];
    n3d_atomic_t atom_[size_];

    n3d_atomic_t head_;
    n3d_atomic_t tail_;
};
