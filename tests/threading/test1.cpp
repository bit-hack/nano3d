#include <source/n3d_thread.h>

namespace {

    struct my_thread_1_t : public n3d_thread_t {

        uint64_t val_;

        my_thread_1_t()
            : n3d_thread_t()
            , val_(0)
        {
        }

        virtual void thread_func() {
            while (is_active()) {
                val_ += 1;
            }
        }
    };

} // namespace {}

bool test1() {

    my_thread_1_t thread;

    thread.start();

    while (!(thread.val_ & 0xff000000)) {
        n3d_yield();
    }

    thread.stop();

    return true;
}
