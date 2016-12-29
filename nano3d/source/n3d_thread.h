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

    static const long c_unlocked = 0;
    static const long c_locked   = 1;

    n3d_spinlock_t()
        : atom_(c_unlocked)
    {
    }

    ~n3d_spinlock_t()
    {
        n3d_assert(atom_ == c_unlocked);
    }

    bool try_lock() {
        return n3d_atomic_xchg(atom_, c_locked) == c_unlocked;
    }

    void lock() {
        while (!try_lock()) {
            n3d_yield();
        }
        n3d_assert(atom_ == c_locked);
    }

    void unlock() {
        n3d_assert(atom_ == c_locked);
        atom_ = c_unlocked;
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
