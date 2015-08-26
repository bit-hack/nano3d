#include <thread>
#include "n3d_thread.h"
#include "n3d_pipe.h"
#include "n3d_types.h"
#include "n3d_atomic.h"

struct n3d_thread_t::detail_t {

    static n3d_atomic_t next_id_;

    detail_t()
        : active_(1)
        , thread_(nullptr)
        , id_(n3d_atomic_inc(next_id_))
    {
    }

    volatile long active_;
    uint32_t id_;
    std::thread * thread_;
};

n3d_atomic_t n3d_thread_t::detail_t::next_id_ = 0;

n3d_thread_t::n3d_thread_t()
    : detail_( new n3d_thread_t::detail_t ) 
{
    assert(detail_);
}

n3d_thread_t::~n3d_thread_t() {
    stop();
    delete detail_;
    detail_ = nullptr;
}

void n3d_thread_t::start() {
    detail_t & d_ = *checked(detail_);
    assert(!d_.thread_);
    d_.active_ = 1;
    d_.thread_ = new std::thread(trampoline, this);
    assert(d_.thread_);
}

void n3d_thread_t::stop() {
    detail_t & d_ = *checked(detail_);
    d_.active_ = 0;
    if (d_.thread_->joinable())
        d_.thread_->join();
    delete d_.thread_;
    d_.thread_ = nullptr;
}

void n3d_thread_t::trampoline(n3d_thread_t * self) {
    assert(self);
    self->thread_func();
}

void n3d_thread_t::thread_func() {
    detail_t & d_ = *checked(detail_);
    while (d_.active_) {
        std::this_thread::yield();
    }
}

void n3d_yield() {
    //(todo) yield to another thread
}
