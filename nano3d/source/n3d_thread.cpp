#include <thread>
#include "n3d_thread.h"
#include "n3d_pipe.h"
#include "n3d_types.h"
#include "n3d_atomic.h"

struct n3d_thread_t::detail_t {

    static n3d_atomic_t next_id_;

    detail_t()
        : active_(1)
        , id_(n3d_atomic_inc(next_id_))
        , thread_(nullptr)
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
    n3d_assert(detail_);
}

n3d_thread_t::~n3d_thread_t() {
    stop();
    delete detail_;
    detail_ = nullptr;
}

void n3d_thread_t::start() {
    detail_t & d_ = *checked(detail_);
    n3d_assert(!d_.thread_);
    d_.active_ = 1;
    d_.thread_ = new std::thread(trampoline, this);
    n3d_assert(d_.thread_);
}

void n3d_thread_t::stop() {
    detail_t & d_ = *checked(detail_);
    if (d_.thread_) {
        d_.active_ = 0;
        if (d_.thread_->joinable())
            d_.thread_->join();
        delete d_.thread_;
        d_.thread_ = nullptr;
    }
}

void n3d_thread_t::trampoline(n3d_thread_t * self) {
    n3d_assert(self);
    while (self->is_active()) {
        self->thread_func();
    }
}

void n3d_thread_t::thread_func() {
    detail_t & d_ = *checked(detail_);
    while (d_.active_) {
        std::this_thread::yield();
    }
}

bool n3d_thread_t::is_active() const {
    detail_t & d_ = *checked(detail_);
    return d_.active_ != 0;
}

void n3d_yield() {
    std::this_thread::yield();
}

uint32_t n3d_thread_t::get_id() const {
    detail_t & d_ = *checked(detail_);
    return d_.id_;
}
