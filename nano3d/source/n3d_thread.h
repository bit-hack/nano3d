#pragma once

#include "n3d_util.h"
#include "n3d_forward.h"
#include "n3d_atomic.h"

void n3d_yield();

struct n3d_thread_t {

    n3d_thread_t();
    ~n3d_thread_t();
    n3d_thread_t(const n3d_thread_t &) = delete;

    void start();
    void stop ();

    uint32_t get_id() const;

    bool is_active() const;

protected:

    virtual void thread_func();

    static void trampoline(n3d_thread_t *);

    struct detail_t;
    detail_t * detail_;
};

struct n3d_spinlock_t {
    
    n3d_atomic_t atom_;

    n3d_spinlock_t()
        : atom_(0)
    {
    }

    ~n3d_spinlock_t()
    {
        n3d_assert(atom_ == 0);
    }

    bool try_lock() {
        return n3d_atomic_xchg(atom_, 1) == 0;
    }

    void lock() {
        while (!try_lock())
            n3d_yield();
    }

    void unlock() {
        atom_ = 0;
    }
};

struct n3d_scope_spinlock_t {

    n3d_scope_spinlock_t(n3d_spinlock_t & sl, bool aquire=true)
        : lock_(sl) {
        if (aquire)
            lock_.lock();
    }

    ~n3d_scope_spinlock_t() {
        lock_.unlock();
    }

protected:
    n3d_spinlock_t & lock_;
};
