#include <source/n3d_pipe.h>
#include <source/n3d_thread.h>

#include "test_common.h"

namespace {

struct my_thread_t : public n3d_thread_t {

    n3d_pipe_t<uint64_t, 256> pipe_;

    my_thread_t()
        : n3d_thread_t()
        , pipe_()
    {
    }

    virtual void thread_func()
    {
        uint64_t rng = seed() | 1;
        uint64_t last = -1;

        for (uint64_t i = 0; is_active(); ++i) {

            if ((i & 0xffff) == (rng & 0xffff)) {
                sleep(rng & 0xff);
                rand64(rng);
            }

            uint64_t out = 0;
            if (pipe_.pop(out)) {
                if ((out - 1) != last) {
                    printf("BAD SEQUENCE!");
                    n3d_assert(!"Things are out of order");
                }
                last = out;
            }
        }
    }
};

} // namespace {}

bool thread_test_2()
{
    static const uint32_t itterations = 1000000;

    my_thread_t thread;
    thread.start();

    uint64_t rng = seed() | 1;

    for (uint64_t i = 0; i < itterations;) {

        if ((i & 0xffff) == (rng & 0xffff)) {
            sleep(rng & 0xff);
            rand64(rng);
        }

        if (thread.pipe_.push(i)) {
            ++i;
        }
    }

    thread.stop();
    return true;
}
