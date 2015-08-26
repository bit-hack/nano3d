#include <source/n3d_thread.h>
#include <source/n3d_pipe.h>

#include <thread>
#include <chrono>

namespace {

    uint64_t rand64(uint64_t & x) {
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        return x * UINT64_C(2685821657736338717);
    }

    uint64_t seed() {
        auto i = std::chrono::system_clock::now().time_since_epoch();
        return i.count();
    }

    struct my_thread_2_t : public n3d_thread_t {

        n3d_pipe_t<uint64_t, 1024> pipe_;

        my_thread_2_t()
            : n3d_thread_t()
            , pipe_()
        {
        }

        virtual void thread_func() {

            uint64_t rng = seed() | 1;

            uint64_t last = -1;

            for (uint64_t i = 0; is_active(); ++i) {

                if ((i & 0xfff) == (rng & 0xfff)) {
                    std::this_thread::sleep_for(std::chrono::microseconds(rng & 0xff));
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

bool test2() {

    my_thread_2_t thread;
    thread.start();

    uint64_t rng = seed() | 1;

    for (uint64_t i = 0; i < 1000000; ) {

        if ((i & 0xfff) == (rng & 0xfff)) {
            std::this_thread::sleep_for(std::chrono::microseconds(rng & 0xff));
            rand64(rng);
        }

        if (thread.pipe_.push(i)) {
            ++i;
        }
    }

    thread.stop();

    return true;
}
