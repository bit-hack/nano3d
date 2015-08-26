#pragma once

#include "n3d_forward.h"

struct n3d_thread_t {

    n3d_thread_t();
    ~n3d_thread_t();
    n3d_thread_t(const n3d_thread_t &) = delete;

    void start();
    void stop ();

    uint32_t get_id() const;

protected:

    virtual void thread_func();

    static void trampoline(n3d_thread_t *);

    struct detail_t;
    detail_t * detail_;
};

void n3d_yield ();
